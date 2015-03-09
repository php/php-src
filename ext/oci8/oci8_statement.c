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
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@zend.com>                          |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"

#if HAVE_OCI8

#include "php_oci8.h"
#include "php_oci8_int.h"

/* {{{ php_oci_statement_create()
 Create statemend handle and allocate necessary resources */
php_oci_statement *php_oci_statement_create(php_oci_connection *connection, char *query, int query_len)
{
	php_oci_statement *statement;
	sword errstatus;

	connection->errcode = 0; /* retain backwards compat with OCI8 1.4 */

	statement = ecalloc(1,sizeof(php_oci_statement));

	if (!query_len) {
		/* do not allocate stmt handle for refcursors, we'll get it from OCIStmtPrepare2() */
		PHP_OCI_CALL(OCIHandleAlloc, (connection->env, (dvoid **)&(statement->stmt), OCI_HTYPE_STMT, 0, NULL));
	}
		
	PHP_OCI_CALL(OCIHandleAlloc, (connection->env, (dvoid **)&(statement->err), OCI_HTYPE_ERROR, 0, NULL));
	
	if (query_len > 0) {
		PHP_OCI_CALL_RETURN(errstatus, OCIStmtPrepare2,
				(
				 connection->svc,
				 &(statement->stmt),
				 connection->err,
				 (text *)query,
				 query_len,
				 NULL,
				 0,
				 OCI_NTV_SYNTAX,
				 OCI_DEFAULT
				)
		);
#ifdef HAVE_OCI8_DTRACE
		if (DTRACE_OCI8_SQLTEXT_ENABLED()) {
			DTRACE_OCI8_SQLTEXT(connection, connection->client_id, statement, query);
		}
#endif /* HAVE_OCI8_DTRACE */

		if (errstatus != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, errstatus);

			PHP_OCI_CALL(OCIStmtRelease, (statement->stmt, statement->err, NULL, 0, OCI_STRLS_CACHE_DELETE));
			PHP_OCI_CALL(OCIHandleFree,(statement->err, OCI_HTYPE_ERROR));
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			efree(statement);
			return NULL;
		}
	}
	
	if (query && query_len) {
		statement->last_query = estrndup(query, query_len);
		statement->last_query_len = query_len;
	}
	else {
		statement->last_query = NULL;
		statement->last_query_len = 0;
	}

	statement->connection = connection;
	statement->has_data = 0;
	statement->has_descr = 0;
	statement->parent_stmtid = 0;
	statement->impres_child_stmt = NULL;
	statement->impres_count = 0;
	statement->impres_flag = PHP_OCI_IMPRES_UNKNOWN;  /* may or may not have Implicit Result Set children */
	++GC_REFCOUNT(statement->connection->id);

	if (OCI_G(default_prefetch) >= 0) {
		php_oci_statement_set_prefetch(statement, (ub4)OCI_G(default_prefetch));
	} else {
		php_oci_statement_set_prefetch(statement, (ub4)100); /* semi-arbitrary, "sensible default" */
	}
	
	PHP_OCI_REGISTER_RESOURCE(statement, le_statement);

	OCI_G(num_statements)++;
	
	return statement;
}
/* }}} */

/* {{{ php_oci_get_implicit_resultset()
   Fetch implicit result set statement resource */
php_oci_statement *php_oci_get_implicit_resultset(php_oci_statement *statement)
{
#if (OCI_MAJOR_VERSION < 12)
	php_error_docref(NULL, E_WARNING, "Implicit results are available in Oracle Database 12c onwards");
	return NULL;
#else
	void *result;
	ub4   rtype;
	php_oci_statement *statement2;  /* implicit result set statement handle */
	sword errstatus;

	PHP_OCI_CALL_RETURN(errstatus, OCIStmtGetNextResult, (statement->stmt, statement->err, &result, &rtype, OCI_DEFAULT));
	if (errstatus == OCI_NO_DATA) {
		return NULL;
	}

	if (rtype != OCI_RESULT_TYPE_SELECT) {
		/* Only OCI_RESULT_TYPE_SELECT is supported by Oracle DB 12cR1 */
		php_error_docref(NULL, E_WARNING, "Unexpected implicit result type returned from Oracle Database");
		return NULL;
	} else {
		statement2 = ecalloc(1,sizeof(php_oci_statement));

		PHP_OCI_CALL(OCIHandleAlloc, (statement->connection->env, (dvoid **)&(statement2->err), OCI_HTYPE_ERROR, 0, NULL));
		statement2->stmt = (OCIStmt *)result;	
		statement2->parent_stmtid = statement->id;
		statement2->impres_child_stmt = NULL;
		statement2->impres_count = 0;
		statement2->impres_flag = PHP_OCI_IMPRES_IS_CHILD;
		statement2->connection = statement->connection;
		statement2->errcode = 0;
		statement2->last_query = NULL;
		statement2->last_query_len = 0;
		statement2->columns = NULL;
		statement2->binds = NULL;
		statement2->defines = NULL;
		statement2->ncolumns = 0;
		statement2->executed = 0;
		statement2->has_data = 0;
		statement2->has_descr = 0;
		statement2->stmttype = 0;

		Z_ADDREF_P(statement->id);
		Z_ADDREF_P(statement2->connection->id);

		php_oci_statement_set_prefetch(statement2, statement->prefetch_count);
		
		PHP_OCI_REGISTER_RESOURCE(statement2, le_statement);
	
		OCI_G(num_statements)++;
		
		return statement2;
	}
#endif /* OCI_MAJOR_VERSION < 12 */
}
/* }}} */

/* {{{ php_oci_statement_set_prefetch()
 Set prefetch buffer size for the statement */
int php_oci_statement_set_prefetch(php_oci_statement *statement, ub4 prefetch )
{
	sword errstatus;

	if (prefetch > 20000) {
		prefetch = 20000;		/* keep it somewhat sane */
	}

	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, (statement->stmt, OCI_HTYPE_STMT, &prefetch, 0, OCI_ATTR_PREFETCH_ROWS, statement->err));
	
	if (errstatus != OCI_SUCCESS) {
		statement->errcode = php_oci_error(statement->err, errstatus);
		PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
		statement->prefetch_count = 0;
		return 1;
	}
	statement->prefetch_count = prefetch;
	statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	return 0;
}
/* }}} */

/* {{{ php_oci_cleanup_pre_fetch()
   Helper function to cleanup ref-cursors and descriptors from the previous row */
int php_oci_cleanup_pre_fetch(void *data)
{
	php_oci_out_column *outcol = data;

	if (!outcol->is_descr && !outcol->is_cursor)
		return ZEND_HASH_APPLY_KEEP;

	switch(outcol->data_type) {
		case SQLT_CLOB:
		case SQLT_BLOB:
		case SQLT_RDD:
		case SQLT_BFILE:
			if (outcol->descid) {
				zend_list_delete(outcol->descid);
				outcol->descid = 0;
			}
			break;
		case SQLT_RSET:
			if (outcol->stmtid) {
				zend_list_delete(outcol->stmtid);
				outcol->stmtid = 0;
				outcol->nested_statement = NULL;
			}
			break;
		default:
			break;
	}
	return ZEND_HASH_APPLY_KEEP;

}
/* }}} */

/* {{{ php_oci_statement_fetch()
 Fetch a row from the statement */
int php_oci_statement_fetch(php_oci_statement *statement, ub4 nrows)
{
	int i;
	void *handlepp;
	ub4 typep, iterp, idxp;
	ub1 in_outp, piecep;
	zend_bool piecewisecols = 0;
	php_oci_out_column *column;
	sword errstatus;

	statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */

	if (statement->has_descr && statement->columns) {
		zend_hash_apply(statement->columns, (apply_func_t) php_oci_cleanup_pre_fetch);
    }

	PHP_OCI_CALL_RETURN(errstatus, OCIStmtFetch, (statement->stmt, statement->err, nrows, OCI_FETCH_NEXT, OCI_DEFAULT));

	if (errstatus == OCI_NO_DATA || nrows == 0) {
		if (statement->last_query == NULL) {
			/* reset define-list for refcursors */
			if (statement->columns) {
				zend_hash_destroy(statement->columns);
				efree(statement->columns);
				statement->columns = NULL;
				statement->ncolumns = 0;
			}
			statement->executed = 0;
		}

		statement->has_data = 0;

		if (nrows == 0) {
			/* this is exactly what we requested */
			return 0;
		}
		return 1;
	}

	/* reset length for all piecewise columns */
	for (i = 0; i < statement->ncolumns; i++) {
		column = php_oci_statement_get_column(statement, i + 1, NULL, 0);
		if (column && column->piecewise) {
			column->retlen4 = 0;
			piecewisecols = 1;
		}
	}
	
	while (errstatus == OCI_NEED_DATA) {
		if (piecewisecols) {
			PHP_OCI_CALL_RETURN(errstatus,
				OCIStmtGetPieceInfo,
				   (
					statement->stmt,
					statement->err,
					&handlepp,
					&typep,
					&in_outp,
					&iterp,
					&idxp,
					&piecep
				   )
			);

			/* scan through our columns for a piecewise column with a matching handle */
			for (i = 0; i < statement->ncolumns; i++) {
				column = php_oci_statement_get_column(statement, i + 1, NULL, 0);
				if (column && column->piecewise && handlepp == column->oci_define)   {
					if (!column->data) {
						column->data = (text *) ecalloc(1, PHP_OCI_PIECE_SIZE + 1);
					} else {
						column->data = erealloc(column->data, column->retlen4 + PHP_OCI_PIECE_SIZE + 1);
					}
					column->cb_retlen = PHP_OCI_PIECE_SIZE;

					/* and instruct fetch to fetch waiting piece into our buffer */
					PHP_OCI_CALL(OCIStmtSetPieceInfo,
						   (
							(void *) column->oci_define,
							OCI_HTYPE_DEFINE,
							statement->err,
							((char*)column->data) + column->retlen4,
							&(column->cb_retlen),
							piecep,
							&column->indicator,
							&column->retcode
						   )
					);
				}
			}
		}

		PHP_OCI_CALL_RETURN(errstatus, OCIStmtFetch, (statement->stmt, statement->err, nrows, OCI_FETCH_NEXT, OCI_DEFAULT));

		if (piecewisecols) {
			for (i = 0; i < statement->ncolumns; i++) {
				column = php_oci_statement_get_column(statement, i + 1, NULL, 0);
				if (column && column->piecewise && handlepp == column->oci_define)	{
					column->retlen4 += column->cb_retlen;
				}
			}
		}
	}

	if (errstatus == OCI_SUCCESS_WITH_INFO || errstatus == OCI_SUCCESS) {
		statement->has_data = 1;

		/* do the stuff needed for OCIDefineByName */
		for (i = 0; i < statement->ncolumns; i++) {
			column = php_oci_statement_get_column(statement, i + 1, NULL, 0);
			if (column == NULL) {
				continue;
			}
			
			if (!column->define) {
				continue;
			}
			
			zval_dtor(&column->define->zval);
			php_oci_column_to_zval(column, &column->define->zval, 0);
		}

		return 0;
	}

	statement->errcode = php_oci_error(statement->err, errstatus);
	PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);

	statement->has_data = 0;

	return 1;
}
/* }}} */

/* {{{ php_oci_statement_get_column()
 Get column from the result set */
php_oci_out_column *php_oci_statement_get_column(php_oci_statement *statement, zend_long column_index, char *column_name, int column_name_len)
{
	php_oci_out_column *column = NULL;
	int i;

	if (statement->columns == NULL) { /* we release the columns at the end of a fetch */
		return NULL;
	}

	if (column_name) {
		for (i = 0; i < statement->ncolumns; i++) {
			column = php_oci_statement_get_column(statement, i + 1, NULL, 0);
			if (column == NULL) {
				continue;
			} else if (((int) column->name_len == column_name_len) && (!strncmp(column->name, column_name, column_name_len))) {
				return column;
			}
		}
	} else if (column_index != -1) {
		if ((column = zend_hash_index_find_ptr(statement->columns, column_index)) == NULL) {
			return NULL;
		}
		return column;
	}

	return NULL;
}
/* }}} */

/* {{{ php_oci_define_callback() */
sb4 php_oci_define_callback(dvoid *ctx, OCIDefine *define, ub4 iter, dvoid **bufpp, ub4 **alenpp, ub1 *piecep, dvoid **indpp, ub2 **rcpp)
{
	php_oci_out_column *outcol = (php_oci_out_column *)ctx;

	if (!outcol) {
		
		php_error_docref(NULL, E_WARNING, "Invalid context pointer value");
		return OCI_ERROR;
	}
	
	switch(outcol->data_type) {
		case SQLT_RSET: {
				php_oci_statement *nested_stmt;

				nested_stmt = php_oci_statement_create(outcol->statement->connection, NULL, 0);
				if (!nested_stmt) {
					return OCI_ERROR;
				}
				nested_stmt->parent_stmtid = outcol->statement->id;
				++GC_REFCOUNT(outcol->statement->id);
				outcol->nested_statement = nested_stmt;
				outcol->stmtid = nested_stmt->id;

				*bufpp = nested_stmt->stmt;
				*alenpp = &(outcol->retlen4);
				*piecep = OCI_ONE_PIECE;
				*indpp = &(outcol->indicator);
				*rcpp = &(outcol->retcode);
				return OCI_CONTINUE;
			}
			break;
		case SQLT_RDD:
		case SQLT_BLOB:
		case SQLT_CLOB:
		case SQLT_BFILE: {
				php_oci_descriptor *descr;
				int dtype;

				if (outcol->data_type == SQLT_BFILE) {
					dtype = OCI_DTYPE_FILE;
				} else if (outcol->data_type == SQLT_RDD ) {
					dtype = OCI_DTYPE_ROWID;
				} else {
					dtype = OCI_DTYPE_LOB;
				}

				descr = php_oci_lob_create(outcol->statement->connection, dtype);
				if (!descr) {
					return OCI_ERROR;
				}
				outcol->descid = descr->id;
				descr->charset_form = outcol->charset_form;
				
				*bufpp = descr->descriptor;
				*alenpp = &(outcol->retlen4);
				*piecep = OCI_ONE_PIECE;
				*indpp = &(outcol->indicator);
				*rcpp = &(outcol->retcode);

				return OCI_CONTINUE;
			}
			break;
	}
	return OCI_ERROR;
}
/* }}} */

/* {{{ php_oci_statement_execute()
 Execute statement */
int php_oci_statement_execute(php_oci_statement *statement, ub4 mode)
{
	php_oci_out_column *outcol;
	php_oci_out_column column;
	OCIParam *param = NULL;
	text *colname;
	ub4 counter;
	ub2 define_type;
	ub4 iters;
	ub4 colcount;
	ub2 dynamic;
	dvoid *buf;
	sword errstatus;

	switch (mode) {
		case OCI_COMMIT_ON_SUCCESS:
		case OCI_DESCRIBE_ONLY:
		case OCI_DEFAULT:
			/* only these are allowed */
#ifdef HAVE_OCI8_DTRACE
			if (DTRACE_OCI8_EXECUTE_MODE_ENABLED()) {
				DTRACE_OCI8_EXECUTE_MODE(statement->connection, statement->connection->client_id, statement, mode);
			}
#endif /* HAVE_OCI8_DTRACE */
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Invalid execute mode given: %d", mode);
			return 1;
			break;
	}
	
	if (!statement->stmttype) {
		/* get statement type */
		PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (ub2 *)&statement->stmttype, (ub4 *)0, OCI_ATTR_STMT_TYPE, statement->err));

		if (errstatus != OCI_SUCCESS) {
			statement->errcode = php_oci_error(statement->err, errstatus);
			PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
			return 1;
		} else {
			statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
		}
	}

	if (statement->stmttype == OCI_STMT_SELECT) {
		iters = 0;
	} else {
		iters = 1;
	}
	
	if (statement->last_query) { /* Don't execute REFCURSORS or Implicit Result Set handles */

		if (statement->binds) {
			int result = 0;
			zend_hash_apply_with_argument(statement->binds, (apply_func_arg_t) php_oci_bind_pre_exec, (void *)&result);
			if (result) {
				return 1;
			}
		}

		/* execute statement */
		PHP_OCI_CALL_RETURN(errstatus, OCIStmtExecute, (statement->connection->svc, statement->stmt, statement->err, iters, 0, NULL, NULL, mode));

		if (errstatus != OCI_SUCCESS) {
			statement->errcode = php_oci_error(statement->err, errstatus);
			PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
			return 1;
		}
		
		if (statement->binds) {
			zend_hash_apply(statement->binds, (apply_func_t) php_oci_bind_post_exec);
		}

		if (mode & OCI_COMMIT_ON_SUCCESS) {
			/* No need to rollback on disconnect */
			statement->connection->rb_on_disconnect = 0;
		} else if (statement->stmttype != OCI_STMT_SELECT) {
			/* Assume some uncommitted DML occurred */
			statement->connection->rb_on_disconnect = 1;
		}
		/* else for SELECT with OCI_NO_AUTO_COMMIT, leave
		 * "rb_on_disconnect" at its previous value.  SELECT can't
		 * initiate uncommitted DML. (An AUTONOMOUS_TRANSACTION in
		 * invoked PL/SQL must explicitly rollback/commit else the
		 * SELECT fails).
		 */

		statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	}

	if (statement->stmttype == OCI_STMT_SELECT && statement->executed == 0) {
		/* we only need to do the define step is this very statement is executed the first time! */
		statement->executed = 1;
		
		ALLOC_HASHTABLE(statement->columns);
		zend_hash_init(statement->columns, 13, NULL, php_oci_column_hash_dtor, 0);
		
		counter = 1;

		/* get number of columns */
		PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (dvoid *)&colcount, (ub4 *)0, OCI_ATTR_PARAM_COUNT, statement->err));
		
		if (errstatus != OCI_SUCCESS) {
			statement->errcode = php_oci_error(statement->err, errstatus);
			PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
			return 1;
		}

		statement->ncolumns = colcount;
		
		for (counter = 1; counter <= colcount; counter++) {
			memset(&column,0,sizeof(php_oci_out_column));
			
			if ((outcol = zend_hash_index_update_ptr(statement->columns, counter, &column)) == NULL) {
				efree(statement->columns);
				/* out of memory */
				return 1;
			}
			
			/* get column */
			PHP_OCI_CALL_RETURN(errstatus, OCIParamGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, statement->err, (dvoid**)&param, counter));
			
			if (errstatus != OCI_SUCCESS) {
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}

			/* get column datatype */
			PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)param, OCI_DTYPE_PARAM, (dvoid *)&outcol->data_type, (ub4 *)0, OCI_ATTR_DATA_TYPE, statement->err));

			if (errstatus != OCI_SUCCESS) {
				PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}

			/* get character set form  */
			PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)param, OCI_DTYPE_PARAM, (dvoid *)&outcol->charset_form, (ub4 *)0, OCI_ATTR_CHARSET_FORM, statement->err));

			if (errstatus != OCI_SUCCESS) {
				PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}
	
			/* get character set id	 */
			PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)param, OCI_DTYPE_PARAM, (dvoid *)&outcol->charset_id, (ub4 *)0, OCI_ATTR_CHARSET_ID, statement->err));

			if (errstatus != OCI_SUCCESS) {
				PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}
	
			/* get size of the column */
			PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)param, OCI_DTYPE_PARAM, (dvoid *)&outcol->data_size, (dvoid *)0, OCI_ATTR_DATA_SIZE, statement->err));
			
			if (errstatus != OCI_SUCCESS) {
				PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}

			outcol->storage_size4 = outcol->data_size;
			outcol->retlen = outcol->data_size;

			/* get scale of the column */
			PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)param, OCI_DTYPE_PARAM, (dvoid *)&outcol->scale, (dvoid *)0, OCI_ATTR_SCALE, statement->err));
			
			if (errstatus != OCI_SUCCESS) {
				PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}

			/* get precision of the column */
			PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)param, OCI_DTYPE_PARAM, (dvoid *)&outcol->precision, (dvoid *)0, OCI_ATTR_PRECISION, statement->err));
			
			if (errstatus != OCI_SUCCESS) {
				PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}
			
			/* get name of the column */
			PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)param, OCI_DTYPE_PARAM, (dvoid **)&colname, (ub4 *)&outcol->name_len, (ub4)OCI_ATTR_NAME, statement->err));
			
			if (errstatus != OCI_SUCCESS) {
				PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}
			PHP_OCI_CALL(OCIDescriptorFree, (param, OCI_DTYPE_PARAM));

			outcol->name = estrndup((char*) colname, outcol->name_len);

			/* find a user-set define */
			if (statement->defines) {
				if ((outcol->define = zend_hash_str_find_ptr(statement->defines, outcol->name, outcol->name_len)) != NULL) {
					if (outcol->define->type) {
						outcol->data_type = outcol->define->type;
					}
				}
			}

			buf = 0;
			switch (outcol->data_type) {
				case SQLT_RSET:
					outcol->statement = statement; /* parent handle */

					define_type = SQLT_RSET;
					outcol->is_cursor = 1;
					outcol->statement->has_descr = 1;
					outcol->storage_size4 = -1;
					outcol->retlen = -1;
					dynamic = OCI_DYNAMIC_FETCH;
					break;

				case SQLT_RDD:	 /* ROWID */
				case SQLT_BLOB:	 /* binary LOB */
				case SQLT_CLOB:	 /* character LOB */
				case SQLT_BFILE: /* binary file LOB */
					outcol->statement = statement; /* parent handle */

					define_type = outcol->data_type;
					outcol->is_descr = 1;
					outcol->statement->has_descr = 1;
					outcol->storage_size4 = -1;
					outcol->chunk_size = 0;
					dynamic = OCI_DYNAMIC_FETCH;
					break;

				case SQLT_LNG:
				case SQLT_LBI:
					if (outcol->data_type == SQLT_LBI) {
						define_type = SQLT_BIN;
					} else {
						define_type = SQLT_CHR;
					}
					outcol->storage_size4 = PHP_OCI_MAX_DATA_SIZE;
					outcol->piecewise = 1;
					dynamic = OCI_DYNAMIC_FETCH;
					break;

				case SQLT_BIN:
				default:
					define_type = SQLT_CHR;
					if (outcol->data_type == SQLT_BIN) {
						define_type = SQLT_BIN;
					}
					if ((outcol->data_type == SQLT_DAT) || (outcol->data_type == SQLT_NUM)
#ifdef SQLT_TIMESTAMP
						|| (outcol->data_type == SQLT_TIMESTAMP)
#endif
#ifdef SQLT_TIMESTAMP_TZ
						|| (outcol->data_type == SQLT_TIMESTAMP_TZ)
#endif
#ifdef SQLT_TIMESTAMP_LTZ
						|| (outcol->data_type == SQLT_TIMESTAMP_LTZ)
#endif
#ifdef SQLT_INTERVAL_YM
						|| (outcol->data_type == SQLT_INTERVAL_YM)
#endif
#ifdef SQLT_INTERVAL_DS
						|| (outcol->data_type == SQLT_INTERVAL_DS)
#endif
						) {
						outcol->storage_size4 = 512; /* XXX this should fit "most" NLS date-formats and Numbers */
#if defined(SQLT_IBFLOAT) && defined(SQLT_IBDOUBLE)
					} else if (outcol->data_type == SQLT_IBFLOAT || outcol->data_type == SQLT_IBDOUBLE) {
						outcol->storage_size4 = 1024;
#endif
					} else {
						outcol->storage_size4++; /* add one for string terminator */
					}
					
					outcol->storage_size4 *= 3;
					
					dynamic = OCI_DEFAULT;
					buf = outcol->data = (text *) safe_emalloc(1, outcol->storage_size4, 0);
					memset(buf, 0, outcol->storage_size4);
					break;
			}

			if (dynamic == OCI_DYNAMIC_FETCH) {
				PHP_OCI_CALL_RETURN(errstatus,
					OCIDefineByPos,
					(
						statement->stmt,							/* IN/OUT handle to the requested SQL query */
						(OCIDefine **)&outcol->oci_define,			/* IN/OUT pointer to a pointer to a define handle */
						statement->err,								/* IN/OUT An error handle  */
						counter,									/* IN	  position in the select list */
						(dvoid *)NULL,								/* IN/OUT pointer to a buffer */
						outcol->storage_size4,						/* IN	  The size of each valuep buffer in bytes */
						define_type,								/* IN	  The data type */
						(dvoid *)&outcol->indicator,				/* IN	  pointer to an indicator variable or arr */
						(ub2 *)NULL,								/* IN/OUT Pointer to array of length of data fetched */
						(ub2 *)NULL,								/* OUT	  Pointer to array of column-level return codes */
						OCI_DYNAMIC_FETCH							/* IN	  mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
					)
				);

			} else {
				PHP_OCI_CALL_RETURN(errstatus,
					OCIDefineByPos,
					(
						statement->stmt,							/* IN/OUT handle to the requested SQL query */
						(OCIDefine **)&outcol->oci_define,			/* IN/OUT pointer to a pointer to a define handle */
						statement->err,								/* IN/OUT An error handle  */
						counter,									/* IN	  position in the select list */
						(dvoid *)buf,								/* IN/OUT pointer to a buffer */
						outcol->storage_size4,						/* IN	  The size of each valuep buffer in bytes */
						define_type,								/* IN	  The data type */
						(dvoid *)&outcol->indicator,				/* IN	  pointer to an indicator variable or arr */
						(ub2 *)&outcol->retlen,						/* IN/OUT Pointer to array of length of data fetched */
						(ub2 *)&outcol->retcode,					/* OUT	  Pointer to array of column-level return codes */
						OCI_DEFAULT									/* IN	  mode (OCI_DEFAULT, OCI_DYNAMIC_FETCH) */
					)
				);

			}
			
			if (errstatus != OCI_SUCCESS) {
				statement->errcode = php_oci_error(statement->err, errstatus);
				PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
				return 1;
			}

			/* additional OCIDefineDynamic() call */
			switch (outcol->data_type) {
				case SQLT_RSET:
				case SQLT_RDD:
				case SQLT_BLOB:
				case SQLT_CLOB:
				case SQLT_BFILE:
					PHP_OCI_CALL_RETURN(errstatus,
						OCIDefineDynamic,
						(
							outcol->oci_define,
							statement->err,
							(dvoid *)outcol,
							php_oci_define_callback
						)
					);

					if (errstatus != OCI_SUCCESS) {
						statement->errcode = php_oci_error(statement->err, errstatus);
						PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
						return 1;
					}
					break;
			}
		}
		statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	}

	return 0;
}
/* }}} */

/* {{{ php_oci_statement_cancel()
 Cancel statement */
int php_oci_statement_cancel(php_oci_statement *statement)
{
	return php_oci_statement_fetch(statement, 0);
}
/* }}} */

/* {{{ php_oci_statement_free()
 Destroy statement handle and free associated resources */
void php_oci_statement_free(php_oci_statement *statement)
{
	if (statement->stmt) {
		if (statement->last_query_len) { /* FIXME: magical */
			PHP_OCI_CALL(OCIStmtRelease, (statement->stmt, statement->err, NULL, 0, statement->errcode ? OCI_STRLS_CACHE_DELETE : OCI_DEFAULT));
		} else if (statement->impres_flag != PHP_OCI_IMPRES_IS_CHILD) {  /* Oracle doc says don't free Implicit Result Set handles */
			PHP_OCI_CALL(OCIHandleFree, (statement->stmt, OCI_HTYPE_STMT));
		}
		statement->stmt = NULL;
	}

	if (statement->err) {
		PHP_OCI_CALL(OCIHandleFree, (statement->err, OCI_HTYPE_ERROR));
		statement->err = NULL;
	}

	if (statement->last_query) {
		efree(statement->last_query);
	}

	if (statement->columns) {
		zend_hash_destroy(statement->columns);
		efree(statement->columns);
	}

	if (statement->binds) {
		zend_hash_destroy(statement->binds);
		efree(statement->binds);
	}

	if (statement->defines) {
		zend_hash_destroy(statement->defines);
		efree(statement->defines);
	}

	if (statement->parent_stmtid) {
		zend_list_delete(statement->parent_stmtid);
	}

	zend_list_delete(statement->connection->id);
	efree(statement);
	
	OCI_G(num_statements)--;
}
/* }}} */

/* {{{ php_oci_bind_pre_exec()
 Helper function */
int php_oci_bind_pre_exec(void *data, void *result)
{
	php_oci_bind *bind = (php_oci_bind *) data;

	*(int *)result = 0;

	if (Z_TYPE(bind->zval) == IS_ARRAY) {
		/* These checks are currently valid for oci_bind_by_name, not
		 * oci_bind_array_by_name.  Also bind->type and
		 * bind->indicator are not used for oci_bind_array_by_name.
		 */
		return 0;
	}	
	switch (bind->type) {
		case SQLT_NTY:
		case SQLT_BFILEE:
		case SQLT_CFILEE:
		case SQLT_CLOB:
		case SQLT_BLOB:
		case SQLT_RDD:
			if (Z_TYPE(bind->zval) != IS_OBJECT) {
				php_error_docref(NULL, E_WARNING, "Invalid variable used for bind");
				*(int *)result = 1;
			}
			break;
			
		case SQLT_CHR:
		case SQLT_AFC:
		case SQLT_INT:
		case SQLT_NUM:
#if defined(OCI_MAJOR_VERSION) && OCI_MAJOR_VERSION >= 12
		case SQLT_BOL:
#endif
		case SQLT_LBI:
		case SQLT_BIN:
		case SQLT_LNG:
			if (Z_TYPE(bind->zval) == IS_RESOURCE || Z_TYPE(bind->zval) == IS_OBJECT) {
				php_error_docref(NULL, E_WARNING, "Invalid variable used for bind");
				*(int *)result = 1;
			}
			break;

		case SQLT_RSET:
			if (Z_TYPE(bind->zval) != IS_RESOURCE) {
				php_error_docref(NULL, E_WARNING, "Invalid variable used for bind");
				*(int *)result = 1;
			}
			break;
	}

	/* reset all bind stuff to a normal state... */
	bind->indicator = 0;

	return 0;
}
/* }}} */

/* {{{ php_oci_bind_post_exec()
 Helper function */
int php_oci_bind_post_exec(void *data)
{
	php_oci_bind *bind = (php_oci_bind *) data;
	php_oci_connection *connection = bind->parent_statement->connection;
	sword errstatus;

	if (bind->indicator == -1) { /* NULL */
		zval *val = &bind->zval;
		if (Z_TYPE_P(val) == IS_STRING) {
			*Z_STRVAL_P(val) = '\0'; /* XXX avoid warning in debug mode */
		}
		zval_dtor(val);
		ZVAL_NULL(val);
	} else if (Z_TYPE(bind->zval) == IS_STRING
			   && Z_STRLEN(bind->zval) > 0
			   && Z_STRVAL(bind->zval)[ Z_STRLEN(bind->zval) ] != '\0') {
		/* The post- PHP 5.3 feature for "interned" strings disallows
		 * their reallocation but (i) any IN binds either interned or
		 * not should already be null terminated and (ii) for OUT
		 * binds, php_oci_bind_out_callback() should have allocated a
		 * new string that we can modify here.
		 */
		Z_STR(bind->zval) = zend_string_realloc(Z_STR(bind->zval), Z_STRLEN(bind->zval)+1, 0);
		Z_STRVAL(bind->zval)[ Z_STRLEN(bind->zval) ] = '\0';
	} else if (Z_TYPE(bind->zval) == IS_ARRAY) {
		int i;
		zval *entry = NULL;
		HashTable *hash = HASH_OF(&bind->zval);
	
		zend_hash_internal_pointer_reset(hash);

		switch (bind->array.type) {
			case SQLT_NUM:
			case SQLT_INT:
			case SQLT_LNG:
				for (i = 0; i < bind->array.current_length; i++) {
					if ((i < bind->array.old_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
						zval_dtor(entry);
						ZVAL_LONG(entry, ((ub4 *)(bind->array.elements))[i]);
						zend_hash_move_forward(hash);
					} else {
						add_next_index_long(&bind->zval, ((ub4 *)(bind->array.elements))[i]);
					}
				}
				break;
			case SQLT_FLT:
				for (i = 0; i < bind->array.current_length; i++) {
					if ((i < bind->array.old_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
						zval_dtor(entry);
						ZVAL_DOUBLE(entry, ((double *)(bind->array.elements))[i]);
						zend_hash_move_forward(hash);
					} else {
						add_next_index_double(&bind->zval, ((double *)(bind->array.elements))[i]);
					}
				}
				break;
			case SQLT_ODT:
				for (i = 0; i < bind->array.current_length; i++) {
					oratext buff[1024];
					ub4 buff_len = 1024;

					memset((void*)buff,0,sizeof(buff));
							
					if ((i < bind->array.old_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
						PHP_OCI_CALL_RETURN(errstatus, OCIDateToText, (connection->err, &(((OCIDate *)(bind->array.elements))[i]), 0, 0, 0, 0, &buff_len, buff));
						zval_dtor(entry);

						if (errstatus != OCI_SUCCESS) {
							connection->errcode = php_oci_error(connection->err, errstatus);
							PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
							ZVAL_NULL(entry);
						} else {
							connection->errcode = 0; /* retain backwards compat with OCI8 1.4 */
							ZVAL_STRINGL(entry, (char *)buff, buff_len);
						}
						zend_hash_move_forward(hash);
					} else {
						PHP_OCI_CALL_RETURN(errstatus, OCIDateToText, (connection->err, &(((OCIDate *)(bind->array.elements))[i]), 0, 0, 0, 0, &buff_len, buff));
						if (errstatus != OCI_SUCCESS) {
							connection->errcode = php_oci_error(connection->err, errstatus);
							PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
							add_next_index_null(&bind->zval);
						} else {
							connection->errcode = 0; /* retain backwards compat with OCI8 1.4 */
							add_next_index_stringl(&bind->zval, (char *)buff, buff_len);
						}
					}
				}
				break;
	
			case SQLT_AFC:
			case SQLT_CHR:
			case SQLT_VCS:
			case SQLT_AVC:
			case SQLT_STR:
			case SQLT_LVC:
				for (i = 0; i < bind->array.current_length; i++) {
					/* int curr_element_length = strlen(((text *)bind->array.elements)+i*bind->array.max_length); */
					int curr_element_length = bind->array.element_lengths[i];
					if ((i < bind->array.old_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
						zval_dtor(entry);
						ZVAL_STRINGL(entry, (char *)(((text *)bind->array.elements)+i*bind->array.max_length), curr_element_length);
						zend_hash_move_forward(hash);
					} else {
						add_next_index_stringl(&bind->zval, (char *)(((text *)bind->array.elements)+i*bind->array.max_length), curr_element_length);
					}
				}
				break;
		}
	}

	return 0;
}
/* }}} */

/* {{{ php_oci_bind_by_name()
 Bind zval to the given placeholder */
int php_oci_bind_by_name(php_oci_statement *statement, char *name, int name_len, zval *var, zend_long maxlength, ub2 type)
{
	php_oci_collection *bind_collection = NULL;
	php_oci_descriptor *bind_descriptor = NULL;
	php_oci_statement  *bind_statement	= NULL;
	dvoid *oci_desc					= NULL;
	/* dvoid *php_oci_collection		   = NULL; */
	OCIStmt *oci_stmt				= NULL;
	dvoid *bind_data				= NULL;
	php_oci_bind bind, *old_bind, *bindp;
	int mode = OCI_DATA_AT_EXEC;
	sb4 value_sz = -1;
	sword errstatus;

	switch (type) {
		case SQLT_NTY:
		{
			zval *tmp;
			
			if (Z_TYPE_P(var) != IS_OBJECT || (tmp = zend_hash_str_find(Z_OBJPROP_P(var), "collection", sizeof("collection"))) == NULL) {
				php_error_docref(NULL, E_WARNING, "Unable to find collection property");
				return 1;
			}

			PHP_OCI_ZVAL_TO_COLLECTION_EX(tmp, bind_collection);
			value_sz = sizeof(void*);
			mode = OCI_DEFAULT;
		
			if (!bind_collection->collection) {
				return 1;
			}
		}
			break;
		case SQLT_BFILEE:
		case SQLT_CFILEE:
		case SQLT_CLOB:
		case SQLT_BLOB:
		case SQLT_RDD:
		{
			zval *tmp;
			
			if (Z_TYPE_P(var) != IS_OBJECT || (tmp = zend_hash_str_find(Z_OBJPROP_P(var), "descriptor", sizeof("descriptor"))) == NULL) {
				php_error_docref(NULL, E_WARNING, "Unable to find descriptor property");
				return 1;
			}

			PHP_OCI_ZVAL_TO_DESCRIPTOR_EX(tmp, bind_descriptor);

			value_sz = sizeof(void*);
			
			oci_desc = bind_descriptor->descriptor;
			
			if (!oci_desc) {
				return 1;
			}
		}
			break;
			
		case SQLT_INT:
		case SQLT_NUM:
			if (Z_TYPE_P(var) == IS_RESOURCE || Z_TYPE_P(var) == IS_OBJECT) {
				php_error_docref(NULL, E_WARNING, "Invalid variable used for bind");
				return 1;
			}
			convert_to_long(var);
			bind_data = (ub4 *)&Z_LVAL_P(var);
			value_sz = sizeof(ub4);
			mode = OCI_DEFAULT;
			break;
			
		case SQLT_LBI:
		case SQLT_BIN:
		case SQLT_LNG:
		case SQLT_AFC:
		case SQLT_CHR: /* SQLT_CHR is the default value when type was not specified */
			if (Z_TYPE_P(var) == IS_RESOURCE || Z_TYPE_P(var) == IS_OBJECT) {
				php_error_docref(NULL, E_WARNING, "Invalid variable used for bind");
				return 1;
			}
			if (Z_TYPE_P(var) != IS_NULL) {
				convert_to_string(var);
			}
			if (maxlength == -1) {
				value_sz = (Z_TYPE_P(var) == IS_STRING) ? Z_STRLEN_P(var) : 0;
			} else {
				value_sz = maxlength;
			}
			break;

		case SQLT_RSET:
			if (Z_TYPE_P(var) != IS_RESOURCE) {
				php_error_docref(NULL, E_WARNING, "Invalid variable used for bind");
				return 1;
			}
			PHP_OCI_ZVAL_TO_STATEMENT_EX(var, bind_statement);
			value_sz = sizeof(void*);

			oci_stmt = bind_statement->stmt;

			if (!oci_stmt) {
				return 1;
			}
			break;

#if defined(OCI_MAJOR_VERSION) && OCI_MAJOR_VERSION >= 12
		case SQLT_BOL:
			if (Z_TYPE_P(var) == IS_RESOURCE || Z_TYPE_P(var) == IS_OBJECT) {
				php_error_docref(NULL, E_WARNING, "Invalid variable used for bind");
				return 1;
			}
			convert_to_boolean(var);
			bind_data = (int *)&Z_LVAL_P(var);
			value_sz = sizeof(int);

			mode = OCI_DEFAULT;
			break;
#endif

		default:
			php_error_docref(NULL, E_WARNING, "Unknown or unsupported datatype given: %d", (int)type);
			return 1;
			break;
	}
	
	if (value_sz == 0) {
		value_sz = 1;
	}
	
	if (!statement->binds) {
		ALLOC_HASHTABLE(statement->binds);
		zend_hash_init(statement->binds, 13, NULL, php_oci_bind_hash_dtor, 0);
	}

	memset((void*)&bind,0,sizeof(php_oci_bind));
	if ((old_bind = zend_hash_str_find_ptr(statement->binds, name, name_len)) != NULL) {
		bindp = old_bind;
		zval_ptr_dtor(&bindp->zval);
	} else {
		bindp = zend_hash_update_ptr(statement->binds, zend_string_init(name, name_len + 1, 0), &bind);
	}
	
	bindp->descriptor = oci_desc;
	bindp->statement = oci_stmt;
	bindp->parent_statement = statement;
	ZVAL_COPY(&bindp->zval, var);
	bindp->type = type;
	Z_ADDREF_P(var);
	
	PHP_OCI_CALL_RETURN(errstatus,
		OCIBindByName,
		(
			statement->stmt,				 /* statement handle */
			(OCIBind **)&bindp->bind,		 /* bind hdl (will alloc) */
			statement->err,				  	 /* error handle */
			(text*) name,					 /* placeholder name */					
			name_len,						 /* placeholder length */
			(dvoid *)bind_data,				 /* in/out data */
			value_sz, /* PHP_OCI_MAX_DATA_SIZE, */ /* max size of input/output data */
			type,							 /* in/out data type */
			(dvoid *)&bindp->indicator,		 /* indicator (ignored) */
			(ub2 *)0,						 /* size array (ignored) */
			(ub2 *)&bindp->retcode,			 /* return code (ignored) */
			(ub4)0,							 /* maxarr_len (PL/SQL only?) */
			(ub4 *)0,						 /* actual array size (PL/SQL only?) */
			mode							 /* mode */
		)
	);

	if (errstatus != OCI_SUCCESS) {
		statement->errcode = php_oci_error(statement->err, errstatus);
		PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
		return 1;
	}

	if (mode == OCI_DATA_AT_EXEC) {
		PHP_OCI_CALL_RETURN(errstatus, OCIBindDynamic,
				(
				 bindp->bind,
				 statement->err,
				 (dvoid *)bindp,
				 php_oci_bind_in_callback,
				 (dvoid *)bindp,
				 php_oci_bind_out_callback
				)
		);

		if (errstatus != OCI_SUCCESS) {
			statement->errcode = php_oci_error(statement->err, errstatus);
			PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
			return 1;
		}
	}

	if (type == SQLT_NTY) {
		/* Bind object */
		PHP_OCI_CALL_RETURN(errstatus, OCIBindObject,
				(
				 bindp->bind,
				 statement->err,
				 bind_collection->tdo,
				 (dvoid **) &(bind_collection->collection),
				 (ub4 *) 0,
				 (dvoid **) 0,
				 (ub4 *) 0
				)
		);
		
		if (errstatus) {
			statement->errcode = php_oci_error(statement->err, errstatus);
			PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
			return 1;
		}
	}
	
	statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	return 0;
}
/* }}} */

/* {{{ php_oci_bind_in_callback()
 Callback used when binding LOBs and VARCHARs */
sb4 php_oci_bind_in_callback(
					dvoid *ictxp,	  /* context pointer */
					OCIBind *bindp,	  /* bind handle */
					ub4 iter,		  /* 0-based execute iteration value */
					ub4 index,		  /* index of current array for PL/SQL or row index for SQL */
					dvoid **bufpp,	  /* pointer to data */
					ub4 *alenp,		  /* size after value/piece has been read */
					ub1 *piecep,	  /* which piece */
					dvoid **indpp)	  /* indicator value */
{
	php_oci_bind *phpbind;
	zval *val;

	if (!(phpbind=(php_oci_bind *)ictxp) || !(val = &phpbind->zval)) {
		php_error_docref(NULL, E_WARNING, "Invalid phpbind pointer value");
		return OCI_ERROR;
	}

	if (ZVAL_IS_NULL(val)) {
		/* we're going to insert a NULL column */
		phpbind->indicator = -1;
		*bufpp = 0;
		*alenp = -1;
		*indpp = (dvoid *)&phpbind->indicator;
	} else	if ((phpbind->descriptor == 0) && (phpbind->statement == 0)) {
		/* "normal string bind */
		convert_to_string(val);

		*bufpp = Z_STRVAL_P(val);
		*alenp = Z_STRLEN_P(val);
		*indpp = (dvoid *)&phpbind->indicator;
	} else if (phpbind->statement != 0) {
		/* RSET */
		*bufpp = phpbind->statement;
		*alenp = -1;		/* seems to be allright */
		*indpp = (dvoid *)&phpbind->indicator;
	} else {
		/* descriptor bind */
		*bufpp = phpbind->descriptor;
		*alenp = -1;		/* seems to be allright */
		*indpp = (dvoid *)&phpbind->indicator;
	}

	*piecep = OCI_ONE_PIECE; /* pass all data in one go */

	return OCI_CONTINUE;
}
/* }}} */

/* {{{ php_oci_bind_out_callback()
 Callback used when binding LOBs and VARCHARs */
sb4 php_oci_bind_out_callback(
					dvoid *octxp,	   /* context pointer */
					OCIBind *bindp,	   /* bind handle */
					ub4 iter,		   /* 0-based execute iteration value */
					ub4 index,		   /* index of current array for PL/SQL or row index for SQL */
					dvoid **bufpp,	   /* pointer to data */
					ub4 **alenpp,	   /* size after value/piece has been read */
					ub1 *piecep,	   /* which piece */
					dvoid **indpp,	   /* indicator value */
					ub2 **rcodepp)	   /* return code */
{
	php_oci_bind *phpbind;
	zval *val;
	sb4 retval = OCI_ERROR;

	if (!(phpbind=(php_oci_bind *)octxp) || !(val = &phpbind->zval)) {
		php_error_docref(NULL, E_WARNING, "Invalid phpbind pointer value");
		return retval;
	}

	if (Z_TYPE_P(val) == IS_RESOURCE) {
		/* Processing for ref-cursor out binds */
		if (phpbind->statement != NULL) {
			*bufpp = phpbind->statement;
			*alenpp = &phpbind->dummy_len;
			*piecep = OCI_ONE_PIECE;
			*rcodepp = &phpbind->retcode;
			*indpp = &phpbind->indicator;
		}
		retval = OCI_CONTINUE;
	} else if (Z_TYPE_P(val) == IS_OBJECT) {
		zval *tmp;
		php_oci_descriptor *desc;

		if (!phpbind->descriptor) {
			return OCI_ERROR;
		}

		/* Do not use the cached lob size if the descriptor is an
		 * out-bind as the contents would have been changed for in/out
		 * binds (Bug #46994).
		 */
		if ((tmp = zend_hash_str_find(Z_OBJPROP_P(val), "descriptor", sizeof("descriptor"))) == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to find object outbind descriptor property");
			return OCI_ERROR;
		}
		PHP_OCI_ZVAL_TO_DESCRIPTOR_EX(tmp, desc);
		desc->lob_size = -1;	/* force OCI8 to update cached size */

		*alenpp = &phpbind->dummy_len;
		*bufpp = phpbind->descriptor;
		*piecep = OCI_ONE_PIECE;
		*rcodepp = &phpbind->retcode;
		*indpp = &phpbind->indicator;
		retval = OCI_CONTINUE;
	} else {
		convert_to_string(val);
		zval_dtor(val);
		
		//Z_STRLEN_P(val) = PHP_OCI_PIECE_SIZE; /* 64K-1 is max XXX */
		//Z_STRVAL_P(val) = ecalloc(1, Z_STRLEN_P(phpbind->zval) + 1);
		// XXX is this right?
		ZVAL_STRINGL(val, NULL, Z_STRLEN(phpbind->zval) + 1);

		/* XXX we assume that zend-zval len has 4 bytes */
		*alenpp = (ub4*) &Z_STRLEN(phpbind->zval);
		*bufpp = Z_STRVAL(phpbind->zval);
		*piecep = OCI_ONE_PIECE;
		*rcodepp = &phpbind->retcode;
		*indpp = &phpbind->indicator;
		retval = OCI_CONTINUE;
	}

	return retval;
}
/* }}} */

/* {{{ php_oci_statement_get_column_helper()
 Helper function to get column by name and index */
php_oci_out_column *php_oci_statement_get_column_helper(INTERNAL_FUNCTION_PARAMETERS, int need_data)
{
	zval *z_statement, *column_index;
	php_oci_statement *statement;
	php_oci_out_column *column;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &z_statement, &column_index) == FAILURE) {
		return NULL;
	}

	statement = (php_oci_statement *) zend_fetch_resource(z_statement, -1, "oci8 statement", NULL, 1, le_statement);

	if (!statement) {
		return NULL;
	}

	if (need_data && !statement->has_data) {
		return NULL;
	}
	
	if (Z_TYPE_P(column_index) == IS_STRING) {
		column = php_oci_statement_get_column(statement, -1, Z_STRVAL_P(column_index), Z_STRLEN_P(column_index));
		if (!column) {
			php_error_docref(NULL, E_WARNING, "Invalid column name \"%s\"", Z_STRVAL_P(column_index));
			return NULL;
		}
	} else {
		zval tmp;
		/* NB: for PHP4 compat only, it should be using 'Z' instead */
		tmp = *column_index;
		zval_copy_ctor(&tmp);
		convert_to_long(&tmp);
		column = php_oci_statement_get_column(statement, Z_LVAL(tmp), NULL, 0);
		if (!column) {
			php_error_docref(NULL, E_WARNING, "Invalid column index \"%pd\"", Z_LVAL(tmp));
			zval_dtor(&tmp);
			return NULL;
		}
		zval_dtor(&tmp);
	}
	return column;
}
/* }}} */

/* {{{ php_oci_statement_get_type()
 Return type of the statement */
int php_oci_statement_get_type(php_oci_statement *statement, ub2 *type)
{
	ub2 statement_type;
	sword errstatus;
	
	*type = 0;
	
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (ub2 *)&statement_type, (ub4 *)0, OCI_ATTR_STMT_TYPE, statement->err));

	if (errstatus != OCI_SUCCESS) {
		statement->errcode = php_oci_error(statement->err, errstatus);
		PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
		return 1;
	}
	statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	*type = statement_type;

	return 0;
}
/* }}} */

/* {{{ php_oci_statement_get_numrows()
 Get the number of rows fetched to the clientside (NOT the number of rows in the result set) */
int php_oci_statement_get_numrows(php_oci_statement *statement, ub4 *numrows)
{
	ub4 statement_numrows;
	sword errstatus;
	
	*numrows = 0;
	
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrGet, ((dvoid *)statement->stmt, OCI_HTYPE_STMT, (ub4 *)&statement_numrows, (ub4 *)0, OCI_ATTR_ROW_COUNT, statement->err));

	if (errstatus != OCI_SUCCESS) {
		statement->errcode = php_oci_error(statement->err, errstatus);
		PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
		return 1;
	}
	statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	*numrows = statement_numrows;

	return 0;
}
/* }}} */

/* {{{ php_oci_bind_array_by_name()
 Bind arrays to PL/SQL types */
int php_oci_bind_array_by_name(php_oci_statement *statement, char *name, int name_len, zval *var, zend_long max_table_length, zend_long maxlength, zend_long type)
{
	php_oci_bind *bind, *bindp;
	sword errstatus;

	convert_to_array(var);

	if (maxlength < -1) {
		php_error_docref(NULL, E_WARNING, "Invalid max length value (%pd)", maxlength);
		return 1;
	}
	
	switch(type) {
		case SQLT_NUM:
		case SQLT_INT:
		case SQLT_LNG:
			bind = php_oci_bind_array_helper_number(var, max_table_length);
			break;

		case SQLT_FLT:
			bind = php_oci_bind_array_helper_double(var, max_table_length);
			break;
			
		case SQLT_AFC:
		case SQLT_CHR:
		case SQLT_VCS:
		case SQLT_AVC:
		case SQLT_STR:
		case SQLT_LVC:
			if (maxlength == -1 && zend_hash_num_elements(Z_ARRVAL_P(var)) == 0) {
				php_error_docref(NULL, E_WARNING, "You must provide max length value for empty arrays");
				return 1;
			}
			bind = php_oci_bind_array_helper_string(var, max_table_length, maxlength);
			break;
		case SQLT_ODT:
			bind = php_oci_bind_array_helper_date(var, max_table_length, statement->connection);
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown or unsupported datatype given: %pd", type);
			return 1;
			break;
	}

	if (bind == NULL) {
		/* failed to generate bind struct */
		return 1;
	}
	
	if (!statement->binds) {
		ALLOC_HASHTABLE(statement->binds);
		zend_hash_init(statement->binds, 13, NULL, php_oci_bind_hash_dtor, 0);
	}

	bindp = zend_hash_update_ptr(statement->binds, zend_string_init(name, name_len + 1, 0), bind);

	bindp->descriptor = NULL;
	bindp->statement = NULL;
	bindp->parent_statement = statement;
	bindp->bind = NULL;
	ZVAL_COPY(&bindp->zval, var);
	bindp->array.type = type;
	bindp->indicator = 0;  		/* not used for array binds */
	bindp->type = 0; 			/* not used for array binds */

	Z_ADDREF_P(var);

	PHP_OCI_CALL_RETURN(errstatus,
							OCIBindByName,
							(
								statement->stmt,
								(OCIBind **)&bindp->bind,
								statement->err,
								(text *)name,
								name_len,
								(dvoid *) bindp->array.elements,
								(sb4) bind->array.max_length,
								(ub2)type,
								(dvoid *)bindp->array.indicators,
								(ub2 *)bind->array.element_lengths,
								(ub2 *)0, /* bindp->array.retcodes, */
								(ub4) max_table_length,
								(ub4 *) &(bindp->array.current_length),
								(ub4) OCI_DEFAULT
							)
						);
	
		
	if (errstatus != OCI_SUCCESS) {
		efree(bind);
		statement->errcode = php_oci_error(statement->err, errstatus);
		PHP_OCI_HANDLE_ERROR(statement->connection, statement->errcode);
		return 1;
	}
	statement->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	efree(bind);
	return 0;
}
/* }}} */

/* {{{ php_oci_bind_array_helper_string()
 Bind arrays to PL/SQL types */
php_oci_bind *php_oci_bind_array_helper_string(zval *var, zend_long max_table_length, zend_long maxlength)
{
	php_oci_bind *bind;
	ub4 i;
	HashTable *hash;
	zval *entry;

	hash = HASH_OF(var);

	if (maxlength == -1) {
		zend_hash_internal_pointer_reset(hash);
		while ((entry = zend_hash_get_current_data(hash)) != NULL) {
			convert_to_string_ex(entry);
			if (Z_STRLEN_P(entry) > maxlength) {
				maxlength = Z_STRLEN_P(entry) + 1;
			}
			zend_hash_move_forward(hash);
		}
	}
	
	bind = emalloc(sizeof(php_oci_bind));
	bind->array.elements		= (text *)safe_emalloc(max_table_length * (maxlength + 1), sizeof(text), 0);
	memset(bind->array.elements, 0, max_table_length * (maxlength + 1) * sizeof(text));
	bind->array.current_length	= zend_hash_num_elements(Z_ARRVAL_P(var));
	bind->array.old_length		= bind->array.current_length;
	bind->array.max_length		= maxlength;
	bind->array.element_lengths	= safe_emalloc(max_table_length, sizeof(ub2), 0);
	memset(bind->array.element_lengths, 0, max_table_length*sizeof(ub2));
	bind->array.indicators		= safe_emalloc(max_table_length, sizeof(sb2), 0);
	memset(bind->array.indicators, 0, max_table_length*sizeof(sb2));
	
	zend_hash_internal_pointer_reset(hash);
	
	for (i = 0; i < bind->array.current_length; i++) {
		if ((entry = zend_hash_get_current_data(hash)) != NULL) {
			convert_to_string_ex(entry);
			bind->array.element_lengths[i] = Z_STRLEN_P(entry);
			if (Z_STRLEN_P(entry) == 0) {
				bind->array.indicators[i] = -1;
			}
			zend_hash_move_forward(hash);
		} else {
			break;
		}
	}

	zend_hash_internal_pointer_reset(hash);
	for (i = 0; i < max_table_length; i++) {
		if ((i < bind->array.current_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
			int element_length;
			
			convert_to_string_ex(entry);
			element_length = (maxlength > Z_STRLEN_P(entry)) ? Z_STRLEN_P(entry) : maxlength;
			
			memcpy((text *)bind->array.elements + i*maxlength, Z_STRVAL_P(entry), element_length);
			((text *)bind->array.elements)[i*maxlength + element_length] = '\0';
			
			zend_hash_move_forward(hash);
		} else {
			((text *)bind->array.elements)[i*maxlength] = '\0';
		}
	}
	zend_hash_internal_pointer_reset(hash);

	return bind;
}
/* }}} */

/* {{{ php_oci_bind_array_helper_number()
 Bind arrays to PL/SQL types */
php_oci_bind *php_oci_bind_array_helper_number(zval *var, zend_long max_table_length)
{
	php_oci_bind *bind;
	ub4 i;
	HashTable *hash;
	zval *entry;

	hash = HASH_OF(var);

	bind = emalloc(sizeof(php_oci_bind));
	bind->array.elements		= (ub4 *)safe_emalloc(max_table_length, sizeof(ub4), 0);
	bind->array.current_length	= zend_hash_num_elements(Z_ARRVAL_P(var));
	bind->array.old_length		= bind->array.current_length;
	bind->array.max_length		= sizeof(ub4);
	bind->array.element_lengths	= safe_emalloc(max_table_length, sizeof(ub2), 0);
	memset(bind->array.element_lengths, 0, max_table_length * sizeof(ub2));
	bind->array.indicators		= NULL;
	
	zend_hash_internal_pointer_reset(hash);
	for (i = 0; i < max_table_length; i++) {
		if (i < bind->array.current_length) {
			bind->array.element_lengths[i] = sizeof(ub4);
		}
		if ((i < bind->array.current_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
			convert_to_long_ex(entry);
			((ub4 *)bind->array.elements)[i] = (ub4) Z_LVAL_P(entry);
			zend_hash_move_forward(hash);
		} else {
			((ub4 *)bind->array.elements)[i] = 0;
		}
	}
	zend_hash_internal_pointer_reset(hash);

	return bind;
}
/* }}} */

/* {{{ php_oci_bind_array_helper_double()
 Bind arrays to PL/SQL types */
php_oci_bind *php_oci_bind_array_helper_double(zval *var, zend_long max_table_length)
{
	php_oci_bind *bind;
	ub4 i;
	HashTable *hash;
	zval *entry;

	hash = HASH_OF(var);

	bind = emalloc(sizeof(php_oci_bind));
	bind->array.elements		= (double *)safe_emalloc(max_table_length, sizeof(double), 0);
	bind->array.current_length	= zend_hash_num_elements(Z_ARRVAL_P(var));
	bind->array.old_length		= bind->array.current_length;
	bind->array.max_length		= sizeof(double);
	bind->array.element_lengths	= safe_emalloc(max_table_length, sizeof(ub2), 0);
	memset(bind->array.element_lengths, 0, max_table_length * sizeof(ub2));
	bind->array.indicators		= NULL;
	
	zend_hash_internal_pointer_reset(hash);
	for (i = 0; i < max_table_length; i++) {
		if (i < bind->array.current_length) {
			bind->array.element_lengths[i] = sizeof(double);
		}
		if ((i < bind->array.current_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
			convert_to_double_ex(entry);
			((double *)bind->array.elements)[i] = (double) Z_DVAL_P(entry);
			zend_hash_move_forward(hash);
		} else {
			((double *)bind->array.elements)[i] = 0;
		}
	}
	zend_hash_internal_pointer_reset(hash);

	return bind;
}
/* }}} */

/* {{{ php_oci_bind_array_helper_date()
 Bind arrays to PL/SQL types */
php_oci_bind *php_oci_bind_array_helper_date(zval *var, zend_long max_table_length, php_oci_connection *connection)
{
	php_oci_bind *bind;
	ub4 i;
	HashTable *hash;
	zval *entry;
	sword errstatus;

	hash = HASH_OF(var);

	bind = emalloc(sizeof(php_oci_bind));
	bind->array.elements		= (OCIDate *)safe_emalloc(max_table_length, sizeof(OCIDate), 0);
	bind->array.current_length	= zend_hash_num_elements(Z_ARRVAL_P(var));
	bind->array.old_length		= bind->array.current_length;
	bind->array.max_length		= sizeof(OCIDate);
	bind->array.element_lengths	= safe_emalloc(max_table_length, sizeof(ub2), 0);
	memset(bind->array.element_lengths, 0, max_table_length * sizeof(ub2));
	bind->array.indicators		= NULL;

	zend_hash_internal_pointer_reset(hash);
	for (i = 0; i < max_table_length; i++) {
		OCIDate oci_date;
		if (i < bind->array.current_length) {
			bind->array.element_lengths[i] = sizeof(OCIDate);
		}
		if ((i < bind->array.current_length) && (entry = zend_hash_get_current_data(hash)) != NULL) {
			
			convert_to_string_ex(entry);
			PHP_OCI_CALL_RETURN(errstatus, OCIDateFromText, (connection->err, (CONST text *)Z_STRVAL_P(entry), Z_STRLEN_P(entry), NULL, 0, NULL, 0, &oci_date));

			if (errstatus != OCI_SUCCESS) {
				/* failed to convert string to date */
				efree(bind->array.element_lengths);
				efree(bind->array.elements);
				efree(bind);
				connection->errcode = php_oci_error(connection->err, errstatus);
				PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
				return NULL;
			}
			
			((OCIDate *)bind->array.elements)[i] = oci_date;
			zend_hash_move_forward(hash);
		} else {
			PHP_OCI_CALL_RETURN(errstatus, OCIDateFromText, (connection->err, (CONST text *)"01-JAN-00", sizeof("01-JAN-00")-1, NULL, 0, NULL, 0, &oci_date));

			if (errstatus != OCI_SUCCESS) {
				/* failed to convert string to date */
				efree(bind->array.element_lengths);
				efree(bind->array.elements);
				efree(bind);
				connection->errcode = php_oci_error(connection->err, errstatus);
				PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
				return NULL;
			}
	
			((OCIDate *)bind->array.elements)[i] = oci_date;
		}
		connection->errcode = 0; /* retain backwards compat with OCI8 1.4 */
	}
	zend_hash_internal_pointer_reset(hash);

	return bind;
}
/* }}} */

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
