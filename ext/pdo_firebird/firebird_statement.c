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

#include <time.h>
	
#define RECORD_ERROR(stmt) _firebird_error(NULL, stmt,  __FILE__, __LINE__ TSRMLS_CC)

/* free the allocated space for passing field values to the db and back */
static void free_sqlda(XSQLDA const *sqlda) /* {{{ */
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
static int firebird_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	int result = 1, i;
	
	/* release the statement */
	if (isc_dsql_free_statement(S->H->isc_status, &S->stmt, DSQL_drop)) {
		RECORD_ERROR(stmt);
		result = 0;
	}

	/* clean up the fetch buffers if they have been used */
	for (i = 0; i < S->out_sqlda.sqld; ++i) {
		if (S->fetch_buf[i]) {
			efree(S->fetch_buf[i]);
		}
	}
	efree(S->fetch_buf);
	
	zend_hash_destroy(S->named_params);
	FREE_HASHTABLE(S->named_params);
	
	/* clean up the input descriptor */
	if (S->in_sqlda) {
		free_sqlda(S->in_sqlda);
		efree(S->in_sqlda);
	}

	free_sqlda(&S->out_sqlda);
	efree(S);
	
	return result;
}
/* }}} */

/* called by PDO to execute a prepared query */
static int firebird_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;

	do {
		/* named cursors should be closed first */
		if (*S->name && isc_dsql_free_statement(H->isc_status, &S->stmt, DSQL_close)) {
			break;
		}
		
		/* assume all params have been bound */
	
		if ((S->statement_type == isc_info_sql_stmt_exec_procedure &&
				isc_dsql_execute2(H->isc_status, &H->tr, &S->stmt, PDO_FB_SQLDA_VERSION,
					S->in_sqlda, &S->out_sqlda))
				|| isc_dsql_execute(H->isc_status, &H->tr, &S->stmt, PDO_FB_SQLDA_VERSION,
					S->in_sqlda)) {
			break;
		}
		
		/* commit? */
		if (stmt->dbh->auto_commit && isc_commit_retaining(H->isc_status, &H->tr)) {
			break;
		}
	
		*S->name = 0;
		S->exhausted = 0;
		
		return 1;
	} while (0);

	RECORD_ERROR(stmt);	

	return 0;
}
/* }}} */

/* called by PDO to fetch the next row from a statement */
static int firebird_stmt_fetch(pdo_stmt_t *stmt, /* {{{ */
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;

	if (!stmt->executed) {
		strcpy(stmt->error_code, "HY000");
		H->last_app_error = "Cannot fetch from a closed cursor";
	} else if (!S->exhausted) {

		/* an EXECUTE PROCEDURE statement can be fetched from once, without calling the API, because
		 * the result was returned in the execute call */
		if (S->statement_type == isc_info_sql_stmt_exec_procedure) {
			S->exhausted = 1;
		} else {
			if (isc_dsql_fetch(H->isc_status, &S->stmt, PDO_FB_SQLDA_VERSION, &S->out_sqlda)) {
				if (H->isc_status[0] && H->isc_status[1]) {
					RECORD_ERROR(stmt);
				}
				S->exhausted = 1;
				return 0;
			}
		}
		return 1;
	}
	return 0;
}
/* }}} */

/* called by PDO to retrieve information about the fields being returned */
static int firebird_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC) /* {{{ */
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
/* }}} */

#if abies_0
/* internal function to override return types of parameters */
static void set_param_type(enum pdo_param_type *param_type, XSQLVAR const *var) /* {{{ */
{
	/* set the param type after the field type */
	switch (var->sqltype & ~1) {
		case SQL_INT64:
#if SIZEOF_LONG < 8
			if (0) /* always a string if its size exceeds native long */
#endif
		case SQL_SHORT:
		case SQL_LONG:
			if (var->sqlscale == 0) {
				*param_type = PDO_PARAM_INT;
				break;
			}
		case SQL_TEXT:
		case SQL_VARYING:
		case SQL_TYPE_DATE:
		case SQL_TYPE_TIME:
		case SQL_TIMESTAMP:
		case SQL_BLOB:
			*param_type = PDO_PARAM_STR;
			break;
		case SQL_FLOAT:
		case SQL_DOUBLE:
			*param_type = PDO_PARAM_DBL;
			break;
	}
}
/* }}} */
#endif

#define FETCH_BUF(buf,type,len,lenvar) ((buf) = (buf) ? (buf) : \
	emalloc((len) ? (len * sizeof(type)) : ((*(unsigned long*)lenvar) = sizeof(type))))

#define CHAR_BUF_LEN 24

/* fetch a blob into a fetch buffer */
static int firebird_fetch_blob(pdo_stmt_t *stmt, int colno, char **ptr, /* {{{ */
	unsigned long *len, ISC_QUAD *blob_id TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;
	isc_blob_handle blobh = NULL;
	char const bl_item = isc_info_blob_total_length;
	char bl_info[20];
	unsigned short i;
	int result = *len = 0;

	if (isc_open_blob(H->isc_status, &H->db, &H->tr, &blobh, blob_id)) {
		RECORD_ERROR(stmt);
		return 0;
	}

	if (isc_blob_info(H->isc_status, &blobh, 1, const_cast(&bl_item),
			sizeof(bl_info), bl_info)) {
		RECORD_ERROR(stmt);
		goto fetch_blob_end;
	}

	/* find total length of blob's data */
	for (i = 0; i < sizeof(bl_info); ) {
		unsigned short item_len;
		char item = bl_info[i++];

		if (item == isc_info_end || item == isc_info_truncated || item == isc_info_error
				|| i >= sizeof(bl_info)) {
			H->last_app_error = "Couldn't determine BLOB size";
			goto fetch_blob_end;
		}								

		item_len = (unsigned short) isc_vax_integer(&bl_info[i], 2);

		if (item == isc_info_blob_total_length) {
			*len = isc_vax_integer(&bl_info[i+2], item_len);
			break;
		}
		i += item_len+2;
	}

	/* we've found the blob's length, now fetch! */
	
	if (*len) {
		unsigned long cur_len;
		unsigned short seg_len;
		ISC_STATUS stat;

		*ptr = S->fetch_buf[colno] = erealloc(*ptr, *len+1);
	
		for (cur_len = stat = 0; (!stat || stat == isc_segment) && cur_len < *len; cur_len += seg_len) {
	
			unsigned short chunk_size = (*len-cur_len) > USHRT_MAX ? USHRT_MAX
				: (unsigned short)(*len-cur_len);
	
			stat = isc_get_segment(H->isc_status, &blobh, &seg_len, chunk_size, &(*ptr)[cur_len]);
		}
	
		(*ptr)[*len++] = '\0';
	
		if (H->isc_status[0] == 1 && (stat != 0 && stat != isc_segstr_eof && stat != isc_segment)) {
			H->last_app_error = "Error reading from BLOB";
			goto fetch_blob_end;
		}
	}
	result = 1;

fetch_blob_end:
	if (isc_close_blob(H->isc_status, &blobh)) {
		RECORD_ERROR(stmt);
		return 0;
	}
	return result;
}
/* }}} */

static int firebird_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr,  /* {{{ */
	unsigned long *len, int *caller_frees TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	XSQLVAR const *var = &S->out_sqlda.sqlvar[colno];

	if (*var->sqlind == -1) {
		/* A NULL value */
		*ptr = NULL;
		*len = 0;
	} else {
		/* override the column param type */
		/* set_param_type(&stmt->columns[colno].param_type,var); */
		stmt->columns[colno].param_type = PDO_PARAM_STR;
		
		if (var->sqlscale < 0) {
			static ISC_INT64 const scales[] = { 1, 10, 100, 1000, 10000, 100000, 1000000,
				100000000, 1000000000, 1000000000, LL_LIT(10000000000),LL_LIT(100000000000),
				LL_LIT(10000000000000), LL_LIT(100000000000000),LL_LIT(1000000000000000),
				LL_LIT(1000000000000000),LL_LIT(1000000000000000000) };
			ISC_INT64 n, f = scales[-var->sqlscale];

			switch (var->sqltype & ~1) {
				case SQL_SHORT:
					n = *(short*)var->sqldata;
					break;
				case SQL_LONG:
					n = *(ISC_LONG*)var->sqldata;
					break;
				case SQL_INT64:
					n = *(ISC_INT64*)var->sqldata;
			}
				
			*ptr = FETCH_BUF(S->fetch_buf[colno], char, CHAR_BUF_LEN, NULL);
			
			if (n >= 0) {
				*len = slprintf(*ptr, CHAR_BUF_LEN, "%" LL_MASK "d.%0*" LL_MASK "d", 
					n / f, -var->sqlscale, n % f);
			} else if (n < -f) {
				*len = slprintf(*ptr, CHAR_BUF_LEN, "%" LL_MASK "d.%0*" LL_MASK "d",
					n / f, -var->sqlscale, -n % f);				
			 } else {
				*len = slprintf(*ptr, CHAR_BUF_LEN, "-0.%0*" LL_MASK "d", -var->sqlscale, -n % f);
			}
		} else {
			switch (var->sqltype & ~1) {
				struct tm t;
				char *fmt;				

/**
* For the time being, this code has been changed to convert every returned value to a string
* before passing it back up to the PDO driver.
*/
				
				case SQL_VARYING:
					*ptr = &var->sqldata[2];
					*len = *(short*)var->sqldata;
					break;
				case SQL_TEXT:
					*ptr = var->sqldata;
					*len = var->sqllen;
					break;
/* --- cut here --- */
				case SQL_SHORT:
				    *ptr = FETCH_BUF(S->fetch_buf[colno], char, CHAR_BUF_LEN, NULL);
					*len = slprintf(*ptr, CHAR_BUF_LEN, "%d", *(short*)var->sqldata);
					break;
				case SQL_LONG:
					*ptr = FETCH_BUF(S->fetch_buf[colno], char, CHAR_BUF_LEN, NULL);
					*len = slprintf(*ptr, CHAR_BUF_LEN, "%ld", *(ISC_LONG*)var->sqldata);
					break;
				case SQL_INT64:
					*ptr = FETCH_BUF(S->fetch_buf[colno], char, CHAR_BUF_LEN, NULL);
					*len = slprintf(*ptr, CHAR_BUF_LEN, "%" LL_MASK "d", *(ISC_INT64*)var->sqldata);
					break;
				case SQL_FLOAT:
					*ptr = FETCH_BUF(S->fetch_buf[colno], char, CHAR_BUF_LEN, NULL);
					*len = slprintf(*ptr, CHAR_BUF_LEN, "%F", *(float*)var->sqldata);
					break;
				case SQL_DOUBLE:
					*ptr = FETCH_BUF(S->fetch_buf[colno], char, CHAR_BUF_LEN, NULL);
					*len = slprintf(*ptr, CHAR_BUF_LEN, "%F" , *(double*)var->sqldata);
					break;
/* --- cut here --- */
#if abies_0
				case SQL_SHORT:
					*ptr = FETCH_BUF(S->fetch_buf[colno], long, 0, *len);
					*(long*)*ptr = *(short*)var->sqldata;
					break;
				case SQL_LONG:
#if SIZEOF_LONG == 8
					*ptr = FETCH_BUF(S->fetch_buf[colno], long, 0, *len);
					*(long*)*ptr = *(ISC_LONG*)var->sqldata;
#else
					*ptr = var->sqldata;
#endif
					break;
				case SQL_INT64:
					*len = sizeof(long);
#if SIZEOF_LONG == 8
					*ptr = var->sqldata;
#else
					*ptr = FETCH_BUF(S->fetch_buf[colno], char, CHAR_BUF_LEN, NULL);
					*len = slprintf(*ptr, CHAR_BUF_LEN, "%" LL_MASK "d", *(ISC_INT64*)var->sqldata);
#endif
					break;
				case SQL_FLOAT:
					*ptr = FETCH_BUF(S->fetch_buf[colno], double, 0, *len);
					*(double*)*ptr = *(float*)var->sqldata;
					break;
				case SQL_DOUBLE:
					*ptr = var->sqldata;
					*len = sizeof(double);
					break;
#endif
				case SQL_TYPE_DATE:
					isc_decode_sql_date((ISC_DATE*)var->sqldata, &t);
					fmt = INI_STR("ibase.dateformat");
					if (0) {
				case SQL_TYPE_TIME:
						isc_decode_sql_time((ISC_TIME*)var->sqldata, &t);
						fmt = INI_STR("ibase.timeformat");
					} else if (0) {
				case SQL_TIMESTAMP:
						isc_decode_timestamp((ISC_TIMESTAMP*)var->sqldata, &t);
						fmt = INI_STR("ibase.timestampformat");
					}

					/* convert the timestamp into a string */
					*ptr = FETCH_BUF(S->fetch_buf[colno], char, *len = 80, NULL);
					*len = strftime(*ptr, *len, fmt, &t);
					break;
				case SQL_BLOB:
					return firebird_fetch_blob(stmt,colno,ptr,len,
						(ISC_QUAD*)var->sqldata TSRMLS_CC);
			}
		}
	}
	return 1;
}
/* }}} */

static int firebird_bind_blob(pdo_stmt_t *stmt, ISC_QUAD *blob_id, zval *param TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	pdo_firebird_db_handle *H = S->H;
	isc_blob_handle h = NULL;
	unsigned long put_cnt = 0, rem_cnt;
	unsigned short chunk_size;
	int result = 1;
	
	if (isc_create_blob(H->isc_status, &H->db, &H->tr, &h, blob_id)) {
		RECORD_ERROR(stmt);
		return 0;
	}

	SEPARATE_ZVAL(&param);

	convert_to_string_ex(&param);
	
	for (rem_cnt = Z_STRLEN_P(param); rem_cnt > 0; rem_cnt -= chunk_size)  {

		chunk_size = rem_cnt > USHRT_MAX ? USHRT_MAX : (unsigned short)rem_cnt;

		if (isc_put_segment(H->isc_status, &h, chunk_size, &Z_STRVAL_P(param)[put_cnt])) {
			RECORD_ERROR(stmt);
			result = 0;
			break;
		}
		put_cnt += chunk_size;
	}
	
	zval_dtor(param);

	if (isc_close_blob(H->isc_status, &h)) {
		RECORD_ERROR(stmt);
		return 0;
	}
	return result;
}	

static int firebird_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, /* {{{ */
	enum pdo_param_event event_type TSRMLS_DC)
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	XSQLDA *sqlda = param->is_param ? S->in_sqlda : &S->out_sqlda;
	XSQLVAR *var;

	if (event_type == PDO_PARAM_EVT_FREE) { /* not used */
		return 1;
	}

	if (!sqlda || param->paramno >= sqlda->sqld) {
		strcpy(stmt->error_code, "HY093");
		S->H->last_app_error = "Invalid parameter index";
		return 0;
	}
	if (param->is_param && param->paramno == -1) {
		long *index;

		/* try to determine the index by looking in the named_params hash */
		if (SUCCESS == zend_hash_find(S->named_params, param->name, param->namelen+1, (void*)&index)) {
			param->paramno = *index;
		} else {
			/* ... or by looking in the input descriptor */
			int i;

			for (i = 0; i < sqlda->sqld; ++i) {
				XSQLVAR *var = &sqlda->sqlvar[i];

				if ((var->aliasname_length && !strncasecmp(param->name, var->aliasname, 
						min(param->namelen, var->aliasname_length))) 
						|| (var->sqlname_length && !strncasecmp(param->name, var->sqlname,
						min(param->namelen, var->sqlname_length)))) {
					param->paramno = i;
					break;
				}
			}
			if (i >= sqlda->sqld) {
				strcpy(stmt->error_code, "HY093");
				S->H->last_app_error = "Invalid parameter name";
				return 0;
			}
		}
	}

	var = &sqlda->sqlvar[param->paramno];
	
	switch (event_type) {
		char *value;
		unsigned long value_len;
		int caller_frees;
			
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

			*var->sqlind = 0;

			switch (var->sqltype & ~1) {
				case SQL_ARRAY:
					strcpy(stmt->error_code, "HY000");
					S->H->last_app_error = "Cannot bind to array field";
					return 0;
	
				case SQL_BLOB:
					return firebird_bind_blob(stmt, (ISC_QUAD*)var->sqldata,
						param->parameter TSRMLS_CC);
			}
							
			/* check if a NULL should be inserted */
			switch (Z_TYPE_P(param->parameter)) {
				int force_null;
				
				case IS_LONG:
					var->sqltype = sizeof(long) == 8 ? SQL_INT64 : SQL_LONG;
					var->sqldata = (void*)&Z_LVAL_P(param->parameter);
					var->sqllen = sizeof(long);
					break;
				case IS_DOUBLE:
					var->sqltype = SQL_DOUBLE;
					var->sqldata = (void*)&Z_DVAL_P(param->parameter);
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
							force_null = (Z_STRLEN_P(param->parameter) == 0);
					}
					if (!force_null) {
						var->sqltype = SQL_TEXT;
						var->sqldata = Z_STRVAL_P(param->parameter);
						var->sqllen	 = Z_STRLEN_P(param->parameter);
						break;
					}
				case IS_NULL:
					/* complain if this field doesn't allow NULL values */
					if (~var->sqltype & 1) {
						strcpy(stmt->error_code, "HY105");
						S->H->last_app_error = "Parameter requires non-null value";
						return 0;
					}
					*var->sqlind = -1;
					break;
				default:
					strcpy(stmt->error_code, "HY105");
					S->H->last_app_error = "Binding arrays/objects is not supported";
					return 0;
			}
			break;

		case PDO_PARAM_EVT_FETCH_POST:
			value = NULL;
			value_len = 0;
			caller_frees = 0;
			
			if (firebird_stmt_get_col(stmt, param->paramno, &value, &value_len, &caller_frees TSRMLS_CC)) {
				switch (PDO_PARAM_TYPE(param->param_type)) {
					case PDO_PARAM_STR:
						if (value) {
							ZVAL_STRINGL(param->parameter, value, value_len, 1);
							break;
						}
					case PDO_PARAM_INT:
						if (value) {
							ZVAL_LONG(param->parameter, *(long*)value);
							break;
						}
#if abies_0
					case PDO_PARAM_DBL:
						if (value) {
							ZVAL_DOUBLE(param->parameter, *(double*)value);
							break;
						}
#endif
					default:
						ZVAL_NULL(param->parameter);
				}
				if (value && caller_frees) {
					efree(value);
				}
				return 1;
			}
			return 0;
		default:
			;
	}		
	return 1;
}
/* }}} */

static int firebird_stmt_set_attribute(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	
	switch (attr) {
		default:
			return 0;
		case PDO_ATTR_CURSOR_NAME:
			convert_to_string(val);
			
			if (isc_dsql_set_cursor_name(S->H->isc_status, &S->stmt, Z_STRVAL_P(val),0)) {
				RECORD_ERROR(stmt);
				return 0;
			}
			strlcpy(S->name, Z_STRVAL_P(val), sizeof(S->name));
			break;
	}
	return 1;
}
/* }}} */

static int firebird_stmt_get_attribute(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC) /* {{{ */
{
	pdo_firebird_stmt *S = (pdo_firebird_stmt*)stmt->driver_data;
	
	switch (attr) {
		default:
			return 0;
		case PDO_ATTR_CURSOR_NAME:
			if (*S->name) {
				ZVAL_STRING(val,S->name,1);
			} else {
				ZVAL_NULL(val);
			}
			break;
	}
	return 1;
}
/* }}} */

struct pdo_stmt_methods firebird_stmt_methods = { /* {{{ */
	firebird_stmt_dtor,
	firebird_stmt_execute,
	firebird_stmt_fetch,
	firebird_stmt_describe,
	firebird_stmt_get_col,
	firebird_stmt_param_hook,
	firebird_stmt_set_attribute,
	firebird_stmt_get_attribute
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
