/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ard Biesheuvel <abies@php.net>                               |
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
#include "php_pdo_firebird.h"
#include "php_pdo_firebird_int.h"

#define RECORD_ERROR(stmt) _firebird_error(NULL, stmt,  __FILE__, __LINE__ TSRMLS_CC)

static void free_sqlda(XSQLDA const *sqlda)
{
	int i;
	
	for (i = 0; i < sqlda->sqld; ++i) {
		XSQLVAR const *var = &sqlda->sqlvar[i];
		
		if (var->sqlind) {
			efree(var->sqlind);
		}
	}
}
	
static int firebird_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	
	if (S->in_sqlda) {
		free_sqlda(S->in_sqlda);
		efree(S->in_sqlda);
	}

	free_sqlda(&S->out_sqlda);
	efree(S);
	
	return 1;
}

static int firebird_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;

	/* named cursors should be closed first */
	if (*S->name && isc_dsql_free_statement(H->isc_status, &S->stmt, DSQL_close)) {
		RECORD_ERROR(stmt);
		return 0;
	}
	
	/* assume all params have been bound */
	if (isc_dsql_execute(H->isc_status, &H->tr, &S->stmt, PDO_FB_SQLDA_VERSION, S->in_sqlda)) {
		RECORD_ERROR(stmt);
		return 0;
	}
	
	S->exhausted = 0;
	
	return 1;
}

static int firebird_stmt_fetch(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;

	switch (S->exhausted) {
		default:
			if (isc_dsql_fetch(H->isc_status, &S->stmt, PDO_FB_SQLDA_VERSION, &S->out_sqlda)) {
				if (H->isc_status[0] && H->isc_status[1]) {
					RECORD_ERROR(stmt);
				}
				S->exhausted = 1;
		case 1:
				return 0;
			}
	}
	return 1;
}

static int firebird_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	struct pdo_column_data *col = &stmt->columns[colno];
	XSQLVAR *var = &S->out_sqlda.sqlvar[colno];
	
	/* allocate storage for the column */
	var->sqlind = (void*)emalloc(var->sqllen + 2*sizeof(short));
	var->sqldata = &((char*)var->sqlind)[sizeof(short)];

	col->precision = -var->sqlscale;
	col->maxlen = var->sqllen;
	col->namelen = var->aliasname_length;
	col->name = estrndup(var->aliasname,var->aliasname_length);
	
	return 1;
}

static void set_param_type(enum pdo_param_type *param_type, XSQLVAR const *var)
{
	/* set the param type after the field type */
	switch (var->sqltype & ~1) {
		case SQL_SHORT:
		case SQL_LONG:
		case SQL_INT64:
			if (var->sqlscale < 0) {
		case SQL_TEXT:
		case SQL_VARYING:
				*param_type = PDO_PARAM_STR;
			} else {
				*param_type = PDO_PARAM_INT;
			}
			break;
		case SQL_FLOAT:
		case SQL_DOUBLE:
			*param_type = PDO_PARAM_DBL;
			break;
	}
}

static int firebird_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
	enum pdo_param_event event_type TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	XSQLDA *sqlda = param->is_param ? S->in_sqlda : &S->out_sqlda;
	XSQLVAR *var = &sqlda->sqlvar[param->paramno];

	if (event_type == PDO_PARAM_EVT_FREE) { /* not used */
		return 1;
	}

	if (!sqlda || param->paramno >= sqlda->sqld) {
		stmt->error_code = PDO_ERR_NOT_FOUND;
		return 0;
	}

	switch (event_type) {
		zval *zparam;
			
		case PDO_PARAM_EVT_ALLOC:
				
			if (param->is_param) {
				/* allocate the parameter */
				var->sqlind = (void*)emalloc(var->sqllen + 2*sizeof(short));
				var->sqldata = &((char*)var->sqlind)[sizeof(short)];
			}
			break;
			
		case PDO_PARAM_EVT_EXEC_PRE:

			if (!param->is_param) {
				break;
			}

			zparam = param->parameter;

			/* check if a NULL should be inserted */
			switch (Z_TYPE_P(zparam)) {
				int force_null;
	
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
							force_null = (Z_STRLEN_P(zparam) == 0);
					}
					if (! force_null) break;
	
				case IS_NULL:
					/* complain if this field doesn't allow NULL values */
					if (! (var->sqltype & 1)) {
						stmt->error_code = PDO_ERR_CONSTRAINT;
						return 0;
					}
					*var->sqlind = -1;
					return 1;
			}

			*var->sqlind = 0;
	
			SEPARATE_ZVAL(&zparam);

			convert_to_string(zparam);

			var->sqltype = SQL_TEXT;
			var->sqldata = Z_STRVAL_P(zparam);
			var->sqllen	 = Z_STRLEN_P(zparam);

			break;

		case PDO_PARAM_EVT_FETCH_POST:
			/* set the param value according to the fetched row */
			return SUCCESS == _php_ibase_var_zval(param->parameter, var->sqldata, var->sqltype,
				var->sqllen, var->sqlscale, 0 TSRMLS_CC);

	}		

	return 1;
}

static int firebird_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	XSQLVAR *var = &S->out_sqlda.sqlvar[colno];

	if (*var->sqlind == -1) {
		/* A NULL value */
		*ptr = NULL;
		*len = 0;
	} else {
		/* override the column param type */
		set_param_type(&stmt->columns[colno].param_type,var);
		
		switch (var->sqltype & ~1) {
			ISC_INT64 bint;
			
			case SQL_VARYING:
				*ptr = &var->sqldata[2];
				*len = *(short*)var->sqldata;
				break;
			case SQL_TEXT:
				*ptr = var->sqldata;
				*len = var->sqllen;
				break;
			case SQL_SHORT:
				*(long*)*ptr = *(short*)var->sqldata;
				*len = sizeof(short);
				break;
			case SQL_LONG:
				*(long*)*ptr = *(ISC_LONG*)var->sqldata;
				*len = sizeof(ISC_LONG);
				break;
			case SQL_INT64:
				*len = sizeof(long);
#if SIZEOF_LONG == 8
				*ptr = var->sqldata;
#else				
				bint = *(ISC_INT64*)var->sqldata;

				if (bint >= LONG_MIN && bint <= LONG_MAX) {
					*(long*)*ptr = (long)bint;
				}
#endif
				break;
		
			case SQL_DOUBLE:
				*ptr = var->sqldata;
				*len = sizeof(double);
				break;
				
			case SQL_TYPE_DATE:
			case SQL_TYPE_TIME:
			case SQL_TIMESTAMP:
				;			
		}
	}
	return 1;
}

static int firebird_stmt_set_attribute(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	
	switch (attr) {
		case PDO_ATTR_CURSOR_NAME:
			convert_to_string(val);
			
			if (isc_dsql_set_cursor_name(S->H->isc_status, &S->stmt, Z_STRVAL_P(val),0)) {
				RECORD_ERROR(stmt);
				return 0;
			}
			strncpy(S->name, Z_STRVAL_P(val), sizeof(S->name));
			S->name[sizeof(S->name)] = 0;
			break;
	}
	return 1;
}

struct pdo_stmt_methods firebird_stmt_methods = {
	firebird_stmt_dtor,
	firebird_stmt_execute,
	firebird_stmt_fetch,
	firebird_stmt_describe,
	firebird_stmt_get_col,
	firebird_stmt_param_hook,
	firebird_stmt_set_attribute
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
