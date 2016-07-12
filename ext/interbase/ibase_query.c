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
   | Authors: Ard Biesheuvel <a.k.biesheuvel@its.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#if HAVE_IBASE

#include "ext/standard/php_standard.h"
#include "php_interbase.h"
#include "php_ibase_includes.h"

#define ISC_LONG_MIN 	INT_MIN
#define ISC_LONG_MAX 	INT_MAX

#define QUERY_RESULT	1
#define EXECUTE_RESULT	2

#define FETCH_ROW		1
#define FETCH_ARRAY		2

typedef struct {
	ISC_ARRAY_DESC ar_desc;
	ISC_LONG ar_size; /* size of entire array in bytes */
	unsigned short el_type, el_size;
} ibase_array;

typedef struct {
	ibase_db_link *link;
	ibase_trans *trans;
	struct _ib_query *query;
	isc_stmt_handle stmt;
	unsigned short type;
	unsigned char has_more_rows, statement_type;
	XSQLDA *out_sqlda;
	ibase_array out_array[1]; /* last member */
} ibase_result;

typedef struct _ib_query {
	ibase_db_link *link;
	ibase_trans *trans;
	ibase_result *result;
	zend_resource *result_res;
	isc_stmt_handle stmt;
	XSQLDA *in_sqlda, *out_sqlda;
	ibase_array *in_array, *out_array;
	unsigned short in_array_cnt, out_array_cnt;
	unsigned short dialect;
	char statement_type;
	char *query;
	zend_resource *trans_res;
} ibase_query;

typedef struct {
	unsigned short vary_length;
	char vary_string[1];
} IBVARY;

/* sql variables union
 * used for convert and binding input variables
 */
typedef struct {
	union {
		short sval;
		float fval;
		ISC_LONG lval;
		ISC_QUAD qval;
		ISC_TIMESTAMP tsval;
		ISC_DATE dtval;
		ISC_TIME tmval;
	} val;
	short sqlind;
} BIND_BUF;

static int le_result, le_query;

#define LE_RESULT "Firebird/InterBase result"
#define LE_QUERY "Firebird/InterBase query"

static void _php_ibase_free_xsqlda(XSQLDA *sqlda) /* {{{ */
{
	int i;
	XSQLVAR *var;

	IBDEBUG("Free XSQLDA?");
	if (sqlda) {
		IBDEBUG("Freeing XSQLDA...");
		var = sqlda->sqlvar;
		for (i = 0; i < sqlda->sqld; i++, var++) {
			efree(var->sqldata);
			if (var->sqlind) {
				efree(var->sqlind);
			}
		}
		efree(sqlda);
	}
}
/* }}} */

static void _php_ibase_free_stmt_handle(ibase_db_link *link, isc_stmt_handle stmt) /* {{{ */
{
	static char info[] = { isc_info_base_level, isc_info_end };

	if (stmt) {
		char res_buf[8];
		IBDEBUG("Dropping statement handle (free_stmt_handle)...");
		/* Only free statement if db-connection is still open */
		if (SUCCESS == isc_database_info(IB_STATUS, &link->handle,
							sizeof(info), info, sizeof(res_buf), res_buf)) {
			if (isc_dsql_free_statement(IB_STATUS, &stmt, DSQL_drop)) {
				_php_ibase_error();
			}
		}
	}
}
/* }}} */

static void _php_ibase_free_result(zend_resource *rsrc) /* {{{ */
{
	ibase_result *ib_result = (ibase_result *) rsrc->ptr;

	IBDEBUG("Freeing result by dtor...");
	if (ib_result) {
		_php_ibase_free_xsqlda(ib_result->out_sqlda);
		if (ib_result->query != NULL) {
			IBDEBUG("query still valid; don't drop statement handle");
			ib_result->query->result = NULL;	/* Indicate to query, that result is released */
		} else {
			_php_ibase_free_stmt_handle(ib_result->link, ib_result->stmt);
		}
		efree(ib_result);
	}
}
/* }}} */

static void _php_ibase_free_query(ibase_query *ib_query) /* {{{ */
{
	IBDEBUG("Freeing query...");

	if (ib_query->in_sqlda) {
		efree(ib_query->in_sqlda);
	}
	if (ib_query->out_sqlda) {
		efree(ib_query->out_sqlda);
	}
	if (ib_query->result != NULL) {
		IBDEBUG("result still valid; don't drop statement handle");
		ib_query->result->query = NULL;	/* Indicate to result, that query is released */
	} else {
		_php_ibase_free_stmt_handle(ib_query->link, ib_query->stmt);
	}
	if (ib_query->in_array) {
		efree(ib_query->in_array);
	}
	if (ib_query->out_array) {
		efree(ib_query->out_array);
	}
	if (ib_query->query) {
		efree(ib_query->query);
	}
}
/* }}} */

static void php_ibase_free_query_rsrc(zend_resource *rsrc) /* {{{ */
{
	ibase_query *ib_query = (ibase_query *)rsrc->ptr;

	if (ib_query != NULL) {
		IBDEBUG("Preparing to free query by dtor...");
		_php_ibase_free_query(ib_query);
		efree(ib_query);
	}
}
/* }}} */

void php_ibase_query_minit(INIT_FUNC_ARGS) /* {{{ */
{
	le_result = zend_register_list_destructors_ex(_php_ibase_free_result, NULL,
	    "interbase result", module_number);
	le_query = zend_register_list_destructors_ex(php_ibase_free_query_rsrc, NULL,
	    "interbase query", module_number);
}
/* }}} */

static int _php_ibase_alloc_array(ibase_array **ib_arrayp, XSQLDA *sqlda, /* {{{ */
	isc_db_handle link, isc_tr_handle trans, unsigned short *array_cnt)
{
	unsigned short i, n;
	ibase_array *ar;

	/* first check if we have any arrays at all */
	for (i = *array_cnt = 0; i < sqlda->sqld; ++i) {
		if ((sqlda->sqlvar[i].sqltype & ~1) == SQL_ARRAY) {
			++*array_cnt;
		}
	}
	if (! *array_cnt) return SUCCESS;

	ar = safe_emalloc(sizeof(ibase_array), *array_cnt, 0);

	for (i = n = 0; i < sqlda->sqld; ++i) {
		unsigned short dim;
		zend_ulong ar_size = 1;
		XSQLVAR *var = &sqlda->sqlvar[i];

		if ((var->sqltype & ~1) == SQL_ARRAY) {
			ibase_array *a = &ar[n++];
			ISC_ARRAY_DESC *ar_desc = &a->ar_desc;

			if (isc_array_lookup_bounds(IB_STATUS, &link, &trans, var->relname,
					var->sqlname, ar_desc)) {
				_php_ibase_error();
				efree(ar);
				return FAILURE;
			}

			switch (ar_desc->array_desc_dtype) {
				case blr_text:
				case blr_text2:
					a->el_type = SQL_TEXT;
					a->el_size = ar_desc->array_desc_length;
					break;
				case blr_short:
					a->el_type = SQL_SHORT;
					a->el_size = sizeof(short);
					break;
				case blr_long:
					a->el_type = SQL_LONG;
					a->el_size = sizeof(ISC_LONG);
					break;
				case blr_float:
					a->el_type = SQL_FLOAT;
					a->el_size = sizeof(float);
					break;
				case blr_double:
					a->el_type = SQL_DOUBLE;
					a->el_size = sizeof(double);
					break;
				case blr_int64:
					a->el_type = SQL_INT64;
					a->el_size = sizeof(ISC_INT64);
					break;
				case blr_timestamp:
					a->el_type = SQL_TIMESTAMP;
					a->el_size = sizeof(ISC_TIMESTAMP);
					break;
				case blr_sql_date:
					a->el_type = SQL_TYPE_DATE;
					a->el_size = sizeof(ISC_DATE);
					break;
				case blr_sql_time:
					a->el_type = SQL_TYPE_TIME;
					a->el_size = sizeof(ISC_TIME);
					break;
				case blr_varying:
				case blr_varying2:
					/**
					 * IB has a strange way of handling VARCHAR arrays. It doesn't store
					 * the length in the first short, as with VARCHAR fields. It does,
					 * however, expect the extra short to be allocated for each element.
					 */
					a->el_type = SQL_TEXT;
					a->el_size = ar_desc->array_desc_length + sizeof(short);
					break;
				case blr_quad:
				case blr_blob_id:
				case blr_cstring:
				case blr_cstring2:
					/**
					 * These types are mentioned as array types in the manual, but I
					 * wouldn't know how to create an array field with any of these
					 * types. I assume these types are not applicable to arrays, and
					 * were mentioned erroneously.
					 */
				default:
					_php_ibase_module_error("Unsupported array type %d in relation '%s' column '%s'",
						ar_desc->array_desc_dtype, var->relname, var->sqlname);
					efree(ar);
					return FAILURE;
			} /* switch array_desc_type */

			/* calculate elements count */
			for (dim = 0; dim < ar_desc->array_desc_dimensions; dim++) {
				ar_size *= 1 + ar_desc->array_desc_bounds[dim].array_bound_upper
					-ar_desc->array_desc_bounds[dim].array_bound_lower;
			}
			a->ar_size = a->el_size * ar_size;
		} /* if SQL_ARRAY */
	} /* for column */
	*ib_arrayp = ar;
	return SUCCESS;
}
/* }}} */

/* allocate and prepare query */
static int _php_ibase_alloc_query(ibase_query *ib_query, ibase_db_link *link, /* {{{ */
	ibase_trans *trans, char *query, unsigned short dialect, zend_resource *trans_res)
{
	static char info_type[] = {isc_info_sql_stmt_type};
	char result[8];

	/* Return FAILURE, if querystring is empty */
	if (*query == '\0') {
		php_error_docref(NULL, E_WARNING, "Querystring empty.");
		return FAILURE;
	}

	ib_query->link = link;
	ib_query->trans = trans;
	ib_query->result_res = NULL;
	ib_query->result = NULL;
	ib_query->stmt = 0;
	ib_query->in_array = NULL;
	ib_query->out_array = NULL;
	ib_query->dialect = dialect;
	ib_query->query = estrdup(query);
	ib_query->trans_res = trans_res;
	ib_query->out_sqlda = NULL;
	ib_query->in_sqlda = NULL;

	if (isc_dsql_allocate_statement(IB_STATUS, &link->handle, &ib_query->stmt)) {
		_php_ibase_error();
		goto _php_ibase_alloc_query_error;
	}

	ib_query->out_sqlda = (XSQLDA *) emalloc(XSQLDA_LENGTH(1));
	ib_query->out_sqlda->sqln = 1;
	ib_query->out_sqlda->version = SQLDA_CURRENT_VERSION;

	if (isc_dsql_prepare(IB_STATUS, &ib_query->trans->handle, &ib_query->stmt,
			0, query, dialect, ib_query->out_sqlda)) {
		_php_ibase_error();
		goto _php_ibase_alloc_query_error;
	}

	/* find out what kind of statement was prepared */
	if (isc_dsql_sql_info(IB_STATUS, &ib_query->stmt, sizeof(info_type),
			info_type, sizeof(result), result)) {
		_php_ibase_error();
		goto _php_ibase_alloc_query_error;
	}
	ib_query->statement_type = result[3];

	/* not enough output variables ? */
	if (ib_query->out_sqlda->sqld > ib_query->out_sqlda->sqln) {
		ib_query->out_sqlda = erealloc(ib_query->out_sqlda, XSQLDA_LENGTH(ib_query->out_sqlda->sqld));
		ib_query->out_sqlda->sqln = ib_query->out_sqlda->sqld;
		ib_query->out_sqlda->version = SQLDA_CURRENT_VERSION;
		if (isc_dsql_describe(IB_STATUS, &ib_query->stmt, SQLDA_CURRENT_VERSION, ib_query->out_sqlda)) {
			_php_ibase_error();
			goto _php_ibase_alloc_query_error;
		}
	}

	/* maybe have input placeholders? */
	ib_query->in_sqlda = emalloc(XSQLDA_LENGTH(1));
	ib_query->in_sqlda->sqln = 1;
	ib_query->in_sqlda->version = SQLDA_CURRENT_VERSION;
	if (isc_dsql_describe_bind(IB_STATUS, &ib_query->stmt, SQLDA_CURRENT_VERSION, ib_query->in_sqlda)) {
		_php_ibase_error();
		goto _php_ibase_alloc_query_error;
	}

	/* not enough input variables ? */
	if (ib_query->in_sqlda->sqln < ib_query->in_sqlda->sqld) {
		ib_query->in_sqlda = erealloc(ib_query->in_sqlda, XSQLDA_LENGTH(ib_query->in_sqlda->sqld));
		ib_query->in_sqlda->sqln = ib_query->in_sqlda->sqld;
		ib_query->in_sqlda->version = SQLDA_CURRENT_VERSION;

		if (isc_dsql_describe_bind(IB_STATUS, &ib_query->stmt,
				SQLDA_CURRENT_VERSION, ib_query->in_sqlda)) {
			_php_ibase_error();
			goto _php_ibase_alloc_query_error;
		}
	}

	/* no, haven't placeholders at all */
	if (ib_query->in_sqlda->sqld == 0) {
		efree(ib_query->in_sqlda);
		ib_query->in_sqlda = NULL;
	} else if (FAILURE == _php_ibase_alloc_array(&ib_query->in_array, ib_query->in_sqlda,
			link->handle, trans->handle, &ib_query->in_array_cnt)) {
		goto _php_ibase_alloc_query_error;
	}

	if (ib_query->out_sqlda->sqld == 0) {
		efree(ib_query->out_sqlda);
		ib_query->out_sqlda = NULL;
	} else 	if (FAILURE == _php_ibase_alloc_array(&ib_query->out_array, ib_query->out_sqlda,
			link->handle, trans->handle, &ib_query->out_array_cnt)) {
		goto _php_ibase_alloc_query_error;
	}

	return SUCCESS;

_php_ibase_alloc_query_error:

	if (ib_query->out_sqlda) {
		efree(ib_query->out_sqlda);
	}
	if (ib_query->in_sqlda) {
		efree(ib_query->in_sqlda);
	}
	if (ib_query->out_array) {
		efree(ib_query->out_array);
	}
	if (ib_query->query) {
		efree(ib_query->query);
	}
	return FAILURE;
}
/* }}} */

static int _php_ibase_bind_array(zval *val, char *buf, zend_ulong buf_size, /* {{{ */
	ibase_array *array, int dim)
{
	zval null_val, *pnull_val = &null_val;
	int u_bound = array->ar_desc.array_desc_bounds[dim].array_bound_upper,
		l_bound = array->ar_desc.array_desc_bounds[dim].array_bound_lower,
		dim_len = 1 + u_bound - l_bound;

	ZVAL_NULL(pnull_val);

	if (dim < array->ar_desc.array_desc_dimensions) {
		zend_ulong slice_size = buf_size / dim_len;
		unsigned short i;
		zval *subval = val;

		if (Z_TYPE_P(val) == IS_ARRAY) {
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(val));
		}

		for (i = 0; i < dim_len; ++i) {

			if (Z_TYPE_P(val) == IS_ARRAY &&
				(subval = zend_hash_get_current_data(Z_ARRVAL_P(val))) == NULL)
			{
				subval = pnull_val;
			}

			if (_php_ibase_bind_array(subval, buf, slice_size, array, dim+1) == FAILURE)
			{
				return FAILURE;
			}
			buf += slice_size;

			if (Z_TYPE_P(val) == IS_ARRAY) {
				zend_hash_move_forward(Z_ARRVAL_P(val));
			}
		}

		if (Z_TYPE_P(val) == IS_ARRAY) {
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(val));
		}

	} else {
		/* expect a single value */
		if (Z_TYPE_P(val) == IS_NULL) {
			memset(buf, 0, buf_size);
		} else if (array->ar_desc.array_desc_scale < 0) {

			/* no coercion for array types */
			double l;

			convert_to_double(val);

			if (Z_DVAL_P(val) > 0) {
				l = Z_DVAL_P(val) * pow(10, -array->ar_desc.array_desc_scale) + .5;
			} else {
				l = Z_DVAL_P(val) * pow(10, -array->ar_desc.array_desc_scale) - .5;
			}

			switch (array->el_type) {
				case SQL_SHORT:
					if (l > SHRT_MAX || l < SHRT_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width");
						return FAILURE;
					}
					*(short*) buf = (short) l;
					break;
				case SQL_LONG:
					if (l > ISC_LONG_MAX || l < ISC_LONG_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width");
						return FAILURE;
					}
					*(ISC_LONG*) buf = (ISC_LONG) l;
					break;
				case SQL_INT64:
					{
						long double l;

						convert_to_string(val);

						if (!sscanf(Z_STRVAL_P(val), "%Lf", &l)) {
							_php_ibase_module_error("Cannot convert '%s' to long double",
								 Z_STRVAL_P(val));
							return FAILURE;
						}

						if (l > 0) {
							*(ISC_INT64 *) buf = (ISC_INT64) (l * pow(10,
								-array->ar_desc.array_desc_scale) + .5);
						} else {
							*(ISC_INT64 *) buf = (ISC_INT64) (l * pow(10,
								-array->ar_desc.array_desc_scale) - .5);
						}
					}
					break;
			}
		} else {
			struct tm t = { 0, 0, 0, 0, 0, 0 };

			switch (array->el_type) {
#ifndef HAVE_STRPTIME
				unsigned short n;
#endif
#if (SIZEOF_ZEND_LONG < 8)
				ISC_INT64 l;
#endif

				case SQL_SHORT:
					convert_to_long(val);
					if (Z_LVAL_P(val) > SHRT_MAX || Z_LVAL_P(val) < SHRT_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width");
						return FAILURE;
					}
					*(short *) buf = (short) Z_LVAL_P(val);
					break;
				case SQL_LONG:
					convert_to_long(val);
#if (SIZEOF_ZEND_LONG > 4)
					if (Z_LVAL_P(val) > ISC_LONG_MAX || Z_LVAL_P(val) < ISC_LONG_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width");
						return FAILURE;
					}
#endif
					*(ISC_LONG *) buf = (ISC_LONG) Z_LVAL_P(val);
					break;
				case SQL_INT64:
#if (SIZEOF_ZEND_LONG >= 8)
					convert_to_long(val);
					*(zend_long *) buf = Z_LVAL_P(val);
#else
					convert_to_string(val);
					if (!sscanf(Z_STRVAL_P(val), "%" LL_MASK "d", &l)) {
						_php_ibase_module_error("Cannot convert '%s' to long integer",
							 Z_STRVAL_P(val));
						return FAILURE;
					} else {
						*(ISC_INT64 *) buf = l;
					}
#endif
					break;
				case SQL_FLOAT:
					convert_to_double(val);
					*(float*) buf = (float) Z_DVAL_P(val);
					break;
				case SQL_DOUBLE:
					convert_to_double(val);
					*(double*) buf = Z_DVAL_P(val);
					break;
				case SQL_TIMESTAMP:
					convert_to_string(val);
#ifdef HAVE_STRPTIME
					strptime(Z_STRVAL_P(val), INI_STR("ibase.timestampformat"), &t);
#else
					n = sscanf(Z_STRVAL_P(val), "%d%*[/]%d%*[/]%d %d%*[:]%d%*[:]%d",
						&t.tm_mon, &t.tm_mday, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec);

					if (n != 3 && n != 6) {
						_php_ibase_module_error("Invalid date/time format (expected 3 or 6 fields, got %d."
							" Use format 'm/d/Y H:i:s'. You gave '%s')", n, Z_STRVAL_P(val));
						return FAILURE;
					}
					t.tm_year -= 1900;
					t.tm_mon--;
#endif
					isc_encode_timestamp(&t, (ISC_TIMESTAMP * ) buf);
					break;
				case SQL_TYPE_DATE:
					convert_to_string(val);
#ifdef HAVE_STRPTIME
					strptime(Z_STRVAL_P(val), INI_STR("ibase.dateformat"), &t);
#else
					n = sscanf(Z_STRVAL_P(val), "%d%*[/]%d%*[/]%d", &t.tm_mon, &t.tm_mday, &t.tm_year);

					if (n != 3) {
						_php_ibase_module_error("Invalid date format (expected 3 fields, got %d. "
							"Use format 'm/d/Y' You gave '%s')", n, Z_STRVAL_P(val));
						return FAILURE;
					}
					t.tm_year -= 1900;
					t.tm_mon--;
#endif
					isc_encode_sql_date(&t, (ISC_DATE *) buf);
					break;
				case SQL_TYPE_TIME:
					convert_to_string(val);
#ifdef HAVE_STRPTIME
					strptime(Z_STRVAL_P(val), INI_STR("ibase.timeformat"), &t);
#else
					n = sscanf(Z_STRVAL_P(val), "%d%*[:]%d%*[:]%d", &t.tm_hour, &t.tm_min, &t.tm_sec);

					if (n != 3) {
						_php_ibase_module_error("Invalid time format (expected 3 fields, got %d. "
							"Use format 'H:i:s'. You gave '%s')", n, Z_STRVAL_P(val));
						return FAILURE;
					}
#endif
					isc_encode_sql_time(&t, (ISC_TIME *) buf);
					break;
				default:
					convert_to_string(val);
					strlcpy(buf, Z_STRVAL_P(val), buf_size);
			}
		}
	}
	return SUCCESS;
}
/* }}} */

static int _php_ibase_bind(XSQLDA *sqlda, zval *b_vars, BIND_BUF *buf, /* {{{ */
	ibase_query *ib_query)
{
	int i, array_cnt = 0, rv = SUCCESS;

	for (i = 0; i < sqlda->sqld; ++i) { /* bound vars */

		zval *b_var = &b_vars[i];
		XSQLVAR *var = &sqlda->sqlvar[i];

		var->sqlind = &buf[i].sqlind;

		/* check if a NULL should be inserted */
		switch (Z_TYPE_P(b_var)) {
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
						force_null = (Z_STRLEN_P(b_var) == 0);
				}

				if (! force_null) break;

			case IS_NULL:
					buf[i].sqlind = -1;

				if (var->sqltype & SQL_ARRAY) ++array_cnt;

				continue;
		}

		/* if we make it to this point, we must provide a value for the parameter */

		buf[i].sqlind = 0;

		var->sqldata = (void*)&buf[i].val;

		switch (var->sqltype & ~1) {
			struct tm t;

			case SQL_TIMESTAMP:
			case SQL_TYPE_DATE:
			case SQL_TYPE_TIME:
				if (Z_TYPE_P(b_var) == IS_LONG) {
					struct tm *res;
					res = php_gmtime_r(&Z_LVAL_P(b_var), &t);
					if (!res) {
						return FAILURE;
					}
				} else {
#ifdef HAVE_STRPTIME
					char *format = INI_STR("ibase.timestampformat");

					convert_to_string(b_var);

					switch (var->sqltype & ~1) {
						case SQL_TYPE_DATE:
							format = INI_STR("ibase.dateformat");
							break;
						case SQL_TYPE_TIME:
							format = INI_STR("ibase.timeformat");
					}
					if (!strptime(Z_STRVAL_P(b_var), format, &t)) {
						/* strptime() cannot handle it, so let IB have a try */
						break;
					}
#else /* ifndef HAVE_STRPTIME */
					break; /* let IB parse it as a string */
#endif
				}

				switch (var->sqltype & ~1) {
					default: /* == case SQL_TIMESTAMP */
						isc_encode_timestamp(&t, &buf[i].val.tsval);
						break;
					case SQL_TYPE_DATE:
						isc_encode_sql_date(&t, &buf[i].val.dtval);
						break;
					case SQL_TYPE_TIME:
						isc_encode_sql_time(&t, &buf[i].val.tmval);
						break;
				}
				continue;

			case SQL_BLOB:

				convert_to_string(b_var);

				if (Z_STRLEN_P(b_var) != BLOB_ID_LEN ||
					!_php_ibase_string_to_quad(Z_STRVAL_P(b_var), &buf[i].val.qval)) {

					ibase_blob ib_blob = { 0, BLOB_INPUT };

					if (isc_create_blob(IB_STATUS, &ib_query->link->handle,
							&ib_query->trans->handle, &ib_blob.bl_handle, &ib_blob.bl_qd)) {
						_php_ibase_error();
						return FAILURE;
					}

					if (_php_ibase_blob_add(b_var, &ib_blob) != SUCCESS) {
						return FAILURE;
					}

					if (isc_close_blob(IB_STATUS, &ib_blob.bl_handle)) {
						_php_ibase_error();
						return FAILURE;
					}
					buf[i].val.qval = ib_blob.bl_qd;
				}
				continue;

			case SQL_ARRAY:

				if (Z_TYPE_P(b_var) != IS_ARRAY) {
					convert_to_string(b_var);

					if (Z_STRLEN_P(b_var) != BLOB_ID_LEN ||
						!_php_ibase_string_to_quad(Z_STRVAL_P(b_var), &buf[i].val.qval)) {

						_php_ibase_module_error("Parameter %d: invalid array ID",i+1);
						rv = FAILURE;
					}
				} else {
					/* convert the array data into something IB can understand */
					ibase_array *ar = &ib_query->in_array[array_cnt];
					void *array_data = emalloc(ar->ar_size);
					ISC_QUAD array_id = { 0, 0 };

					if (FAILURE == _php_ibase_bind_array(b_var, array_data, ar->ar_size,
							ar, 0)) {
						_php_ibase_module_error("Parameter %d: failed to bind array argument", i+1);
						efree(array_data);
						rv = FAILURE;
						continue;
					}

					if (isc_array_put_slice(IB_STATUS, &ib_query->link->handle, &ib_query->trans->handle,
							&array_id, &ar->ar_desc, array_data, &ar->ar_size)) {
						_php_ibase_error();
						efree(array_data);
						return FAILURE;
					}
					buf[i].val.qval = array_id;
					efree(array_data);
				}
				++array_cnt;
				continue;
			} /* switch */

			/* we end up here if none of the switch cases handled the field */
			convert_to_string(b_var);
			var->sqldata = Z_STRVAL_P(b_var);
			var->sqllen	 = Z_STRLEN_P(b_var);
			var->sqltype = SQL_TEXT;
	} /* for */
	return rv;
}
/* }}} */

static void _php_ibase_alloc_xsqlda(XSQLDA *sqlda) /* {{{ */
{
	int i;

	for (i = 0; i < sqlda->sqld; i++) {
		XSQLVAR *var = &sqlda->sqlvar[i];

		switch (var->sqltype & ~1) {
			case SQL_TEXT:
				var->sqldata = safe_emalloc(sizeof(char), var->sqllen, 0);
				break;
			case SQL_VARYING:
				var->sqldata = safe_emalloc(sizeof(char), var->sqllen + sizeof(short), 0);
				break;
			case SQL_SHORT:
				var->sqldata = emalloc(sizeof(short));
				break;
			case SQL_LONG:
				var->sqldata = emalloc(sizeof(ISC_LONG));
				break;
			case SQL_FLOAT:
				var->sqldata = emalloc(sizeof(float));
					break;
			case SQL_DOUBLE:
				var->sqldata = emalloc(sizeof(double));
				break;
			case SQL_INT64:
				var->sqldata = emalloc(sizeof(ISC_INT64));
				break;
			case SQL_TIMESTAMP:
				var->sqldata = emalloc(sizeof(ISC_TIMESTAMP));
				break;
			case SQL_TYPE_DATE:
				var->sqldata = emalloc(sizeof(ISC_DATE));
				break;
			case SQL_TYPE_TIME:
				var->sqldata = emalloc(sizeof(ISC_TIME));
				break;
			case SQL_BLOB:
			case SQL_ARRAY:
				var->sqldata = emalloc(sizeof(ISC_QUAD));
				break;
		} /* switch */

		if (var->sqltype & 1) { /* sql NULL flag */
			var->sqlind = emalloc(sizeof(short));
		} else {
			var->sqlind = NULL;
		}
	} /* for */
}
/* }}} */

static int _php_ibase_exec(INTERNAL_FUNCTION_PARAMETERS, ibase_result **ib_resultp, /* {{{ */
	ibase_query *ib_query, zval *args)
{
	XSQLDA *in_sqlda = NULL, *out_sqlda = NULL;
	BIND_BUF *bind_buf = NULL;
	int i, rv = FAILURE;
	static char info_count[] = { isc_info_sql_records };
	char result[64];
	ISC_STATUS isc_result;
	int argc = ib_query->in_sqlda ? ib_query->in_sqlda->sqld : 0;

	RESET_ERRMSG;

	for (i = 0; i < argc; ++i) {
		SEPARATE_ZVAL(&args[i]);
	}

	switch (ib_query->statement_type) {
		isc_tr_handle tr;
		ibase_tr_list **l;
		ibase_trans *trans;

		case isc_info_sql_stmt_start_trans:

			/* a SET TRANSACTION statement should be executed with a NULL trans handle */
			tr = 0;

			if (isc_dsql_execute_immediate(IB_STATUS, &ib_query->link->handle, &tr, 0,
					ib_query->query, ib_query->dialect, NULL)) {
				_php_ibase_error();
				goto _php_ibase_exec_error;
			}

			trans = (ibase_trans *) emalloc(sizeof(ibase_trans));
			trans->handle = tr;
			trans->link_cnt = 1;
			trans->affected_rows = 0;
			trans->db_link[0] = ib_query->link;

			if (ib_query->link->tr_list == NULL) {
				ib_query->link->tr_list = (ibase_tr_list *) emalloc(sizeof(ibase_tr_list));
				ib_query->link->tr_list->trans = NULL;
				ib_query->link->tr_list->next = NULL;
			}

			/* link the transaction into the connection-transaction list */
			for (l = &ib_query->link->tr_list; *l != NULL; l = &(*l)->next);
			*l = (ibase_tr_list *) emalloc(sizeof(ibase_tr_list));
			(*l)->trans = trans;
			(*l)->next = NULL;

			RETVAL_RES(zend_register_resource(trans, le_trans));
			Z_TRY_ADDREF_P(return_value);

			return SUCCESS;

		case isc_info_sql_stmt_commit:
		case isc_info_sql_stmt_rollback:

			if (isc_dsql_execute_immediate(IB_STATUS, &ib_query->link->handle,
					&ib_query->trans->handle, 0, ib_query->query, ib_query->dialect, NULL)) {
				_php_ibase_error();
				goto _php_ibase_exec_error;
			}

			if (ib_query->trans->handle == 0 && ib_query->trans_res != NULL) {
				/* transaction was released by the query and was a registered resource,
				   so we have to release it */
				zend_list_delete(ib_query->trans_res);
				ib_query->trans_res = NULL;
			}

			RETVAL_TRUE;

			return SUCCESS;

		default:
			RETVAL_FALSE;
	}

	/* allocate sqlda and output buffers */
	if (ib_query->out_sqlda) { /* output variables in select, select for update */
		ibase_result *res;

		IBDEBUG("Query wants XSQLDA for output");
		res = emalloc(sizeof(ibase_result)+sizeof(ibase_array)*max(0,ib_query->out_array_cnt-1));
		res->link = ib_query->link;
		res->trans = ib_query->trans;
		res->stmt = ib_query->stmt;
		/* ib_result and ib_query point at each other to handle release of statement handle properly */
		res->query = ib_query;
		ib_query->result = res;
		res->statement_type = ib_query->statement_type;
		res->has_more_rows = 1;

		out_sqlda = res->out_sqlda = emalloc(XSQLDA_LENGTH(ib_query->out_sqlda->sqld));
		memcpy(out_sqlda, ib_query->out_sqlda, XSQLDA_LENGTH(ib_query->out_sqlda->sqld));
		_php_ibase_alloc_xsqlda(out_sqlda);

		if (ib_query->out_array) {
			memcpy(&res->out_array, ib_query->out_array, sizeof(ibase_array)*ib_query->out_array_cnt);
		}
		*ib_resultp = res;
	}

	if (ib_query->in_sqlda) { /* has placeholders */
		IBDEBUG("Query wants XSQLDA for input");
		in_sqlda = emalloc(XSQLDA_LENGTH(ib_query->in_sqlda->sqld));
		memcpy(in_sqlda, ib_query->in_sqlda, XSQLDA_LENGTH(ib_query->in_sqlda->sqld));
		bind_buf = safe_emalloc(sizeof(BIND_BUF), ib_query->in_sqlda->sqld, 0);
		if (_php_ibase_bind(in_sqlda, args, bind_buf, ib_query) == FAILURE) {
			IBDEBUG("Could not bind input XSQLDA");
			goto _php_ibase_exec_error;
		}
	}

	if (ib_query->statement_type == isc_info_sql_stmt_exec_procedure) {
		isc_result = isc_dsql_execute2(IB_STATUS, &ib_query->trans->handle,
			&ib_query->stmt, SQLDA_CURRENT_VERSION, in_sqlda, out_sqlda);
	} else {
		isc_result = isc_dsql_execute(IB_STATUS, &ib_query->trans->handle,
			&ib_query->stmt, SQLDA_CURRENT_VERSION, in_sqlda);
	}
	if (isc_result) {
		IBDEBUG("Could not execute query");
		_php_ibase_error();
		goto _php_ibase_exec_error;
	}
	ib_query->trans->affected_rows = 0;

	switch (ib_query->statement_type) {

		unsigned long affected_rows;

		case isc_info_sql_stmt_insert:
		case isc_info_sql_stmt_update:
		case isc_info_sql_stmt_delete:
		case isc_info_sql_stmt_exec_procedure:

			if (isc_dsql_sql_info(IB_STATUS, &ib_query->stmt, sizeof(info_count),
					info_count, sizeof(result), result)) {
				_php_ibase_error();
				goto _php_ibase_exec_error;
			}

			affected_rows = 0;

			if (result[0] == isc_info_sql_records) {
				unsigned i = 3, result_size = isc_vax_integer(&result[1],2);

				while (result[i] != isc_info_end && i < result_size) {
					short len = (short)isc_vax_integer(&result[i+1],2);
					if (result[i] != isc_info_req_select_count) {
						affected_rows += isc_vax_integer(&result[i+3],len);
					}
					i += len+3;
				}
			}

			ib_query->trans->affected_rows = affected_rows;

			if (!ib_query->out_sqlda) { /* no result set is being returned */
				if (affected_rows) {
					RETVAL_LONG(affected_rows);
				} else {
					RETVAL_TRUE;
				}
				break;
			}
		default:
			RETVAL_TRUE;
	}

	rv = SUCCESS;

_php_ibase_exec_error:

	if (in_sqlda) {
		efree(in_sqlda);
	}
	if (bind_buf)
		efree(bind_buf);

	if (rv == FAILURE) {
		if (*ib_resultp) {
			efree(*ib_resultp);
			*ib_resultp = NULL;
		}
		if (out_sqlda) {
			_php_ibase_free_xsqlda(out_sqlda);
		}
	}

	return rv;
}
/* }}} */

/* {{{ proto mixed ibase_query([resource link_identifier, [ resource link_identifier, ]] string query [, mixed bind_arg [, mixed bind_arg [, ...]]])
   Execute a query */
PHP_FUNCTION(ibase_query)
{
	zval *zlink, *ztrans, *bind_args = NULL;
	char *query;
	size_t query_len;
	int bind_i, bind_num;
	zend_resource *trans_res = NULL;
	ibase_db_link *ib_link = NULL;
	ibase_trans *trans = NULL;
	ibase_query ib_query = { NULL, NULL, 0, 0 };
	ibase_result *result = NULL;

	RESET_ERRMSG;

	RETVAL_FALSE;

	switch (ZEND_NUM_ARGS()) {
		zend_long l;

		default:
		    if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 3, "rrs",
					&zlink, &ztrans, &query, &query_len)) {

				ib_link = (ibase_db_link*)zend_fetch_resource2_ex(zlink, LE_LINK, le_link, le_plink);
				trans = (ibase_trans*)zend_fetch_resource_ex(ztrans, LE_TRANS,	le_trans);

				trans_res = Z_RES_P(ztrans);
				bind_i = 3;
				break;
		    }
		case 2:
			if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, 2, "rs",
					&zlink, &query, &query_len)) {
				_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, zlink, &ib_link, &trans);

				if (trans != NULL) {
					trans_res = Z_RES_P(zlink);
				}
				bind_i = 2;
				break;
			}

			/* the statement is 'CREATE DATABASE ...' if the link argument is IBASE_CREATE */
			if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(),
					 "ls", &l, &query, &query_len) && l == PHP_IBASE_CREATE) {
				isc_db_handle db = 0;
				isc_tr_handle trans = 0;

				if (PG(sql_safe_mode)) {
					_php_ibase_module_error("CREATE DATABASE is not allowed in SQL safe mode"
						);

				} else if (((l = INI_INT("ibase.max_links")) != -1) && (IBG(num_links) >= l)) {
					_php_ibase_module_error("CREATE DATABASE is not allowed: maximum link count "
						"(" ZEND_LONG_FMT ") reached", l);

				} else if (isc_dsql_execute_immediate(IB_STATUS, &db, &trans, (short)query_len,
						query, SQL_DIALECT_CURRENT, NULL)) {
					_php_ibase_error();

				} else if (!db) {
					_php_ibase_module_error("Connection to created database could not be "
						"established");

				} else {

					/* register the link as a resource; unfortunately, we cannot register
					   it in the hash table, because we don't know the connection params */
					ib_link = (ibase_db_link *) emalloc(sizeof(ibase_db_link));
					ib_link->handle = db;
					ib_link->dialect = SQL_DIALECT_CURRENT;
					ib_link->tr_list = NULL;
					ib_link->event_head = NULL;

					RETVAL_RES(zend_register_resource(ib_link, le_link));
					Z_TRY_ADDREF_P(return_value);
					Z_TRY_ADDREF_P(return_value);
					IBG(default_link) = Z_RES_P(return_value);
				}
				return;
			}
		case 1:
		case 0:
			if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() ? 1 : 0, "s", &query,
					&query_len)) {
				ib_link = (ibase_db_link *)zend_fetch_resource2(IBG(default_link), LE_LINK, le_link, le_plink);

				bind_i = 1;
				break;
			}
			return;
	}

	/* open default transaction */
	if (ib_link == NULL || FAILURE == _php_ibase_def_trans(ib_link, &trans)
			|| FAILURE == _php_ibase_alloc_query(&ib_query, ib_link, trans, query, ib_link->dialect, trans_res)) {
		return;
	}

	do {
		int bind_n = ZEND_NUM_ARGS() - bind_i,
		    expected_n = ib_query.in_sqlda ? ib_query.in_sqlda->sqld : 0;

		if (bind_n != expected_n) {
			php_error_docref(NULL, (bind_n < expected_n) ? E_WARNING : E_NOTICE,
				"Statement expects %d arguments, %d given", expected_n, bind_n);
			if (bind_n < expected_n) {
				break;
			}
		} else if (bind_n > 0) {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &bind_args, &bind_num) == FAILURE) {
				return;
			}
		}

		if (FAILURE == _php_ibase_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, &result, &ib_query,
				&bind_args[bind_i])) {
			break;
		}

		if (result != NULL) { /* statement returns a result */
			result->type = QUERY_RESULT;

			/* EXECUTE PROCEDURE returns only one row => statement can be released immediately */
			if (ib_query.statement_type != isc_info_sql_stmt_exec_procedure) {
				ib_query.stmt = 0; /* keep stmt when free query */
			}
			RETVAL_RES(zend_register_resource(result, le_result));
			Z_TRY_ADDREF_P(return_value);
		}
	} while (0);

	_php_ibase_free_query(&ib_query);

}
/* }}} */

/* {{{ proto int ibase_affected_rows( [ resource link_identifier ] )
   Returns the number of rows affected by the previous INSERT, UPDATE or DELETE statement */
PHP_FUNCTION(ibase_affected_rows)
{
	ibase_trans *trans = NULL;
	ibase_db_link *ib_link;
	zval *arg = NULL;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r", &arg) == FAILURE) {
		return;
	}

	if (!arg) {
		ib_link = (ibase_db_link *)zend_fetch_resource2(IBG(default_link), LE_LINK, le_link, le_plink);
		if (ib_link->tr_list == NULL || ib_link->tr_list->trans == NULL) {
			RETURN_FALSE;
		}
		trans = ib_link->tr_list->trans;
	} else {
		/* one id was passed, could be db or trans id */
		_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, arg, &ib_link, &trans);
		if (trans == NULL) {
			ib_link = (ibase_db_link *)zend_fetch_resource2_ex(arg, LE_LINK, le_link, le_plink);

			if (ib_link->tr_list == NULL || ib_link->tr_list->trans == NULL) {
				RETURN_FALSE;
			}
			trans = ib_link->tr_list->trans;
		}
	}
	RETURN_LONG(trans->affected_rows);
}
/* }}} */

/* {{{ proto int ibase_num_rows( resource result_identifier )
   Return the number of rows that are available in a result */
#if abies_0
PHP_FUNCTION(ibase_num_rows)
{
	/**
	 * As this function relies on the InterBase API function isc_dsql_sql_info()
	 * which has a couple of limitations (which I hope will be fixed in future
	 * releases of Firebird), this function is fairly useless. I'm leaving it
	 * in place for people who can live with the limitations, which I only
	 * found out about after I had implemented it anyway.
	 *
	 * Currently, there's no way to determine how many rows can be fetched from
	 * a cursor. The only number that _can_ be determined is the number of rows
	 * that have already been pre-fetched by the client library.
	 * This implies the following:
	 * - num_rows() always returns zero before the first fetch;
	 * - num_rows() for SELECT ... FOR UPDATE is broken -> never returns a
	 *   higher number than the number of records fetched so far (no pre-fetch);
	 * - the result of num_rows() for other statements is merely a lower bound
	 *   on the number of records => calling ibase_num_rows() again after a couple
	 *   of fetches will most likely return a new (higher) figure for large result
	 *   sets.
	 */

	zval *result_arg;
	ibase_result *ib_result;
	static char info_count[] = {isc_info_sql_records};
	char result[64];

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result_arg) == FAILURE) {
		return;
	}

	ib_result = (ibase_result *)zend_fetch_resource_ex(&result_arg, LE_RESULT, le_result);

	if (isc_dsql_sql_info(IB_STATUS, &ib_result->stmt, sizeof(info_count), info_count, sizeof(result), result)) {
		_php_ibase_error();
		RETURN_FALSE;
	}

	if (result[0] == isc_info_sql_records) {
		unsigned i = 3, result_size = isc_vax_integer(&result[1],2);

		while (result[i] != isc_info_end && i < result_size) {
			short len = (short)isc_vax_integer(&result[i+1],2);
			if (result[i] == isc_info_req_select_count) {
				RETURN_LONG(isc_vax_integer(&result[i+3],len));
			}
			i += len+3;
		}
	}
}
#endif
/* }}} */

static int _php_ibase_var_zval(zval *val, void *data, int type, int len, /* {{{ */
	int scale, int flag)
{
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

	switch (type & ~1) {
		unsigned short l;
		zend_long n;
		char string_data[255];
		struct tm t;
		char *format;

		case SQL_VARYING:
			len = ((IBVARY *) data)->vary_length;
			data = ((IBVARY *) data)->vary_string;
			/* no break */
		case SQL_TEXT:
			ZVAL_STRINGL(val, (char*)data, len);
			break;
		case SQL_SHORT:
			n = *(short *) data;
			goto _sql_long;
		case SQL_INT64:
#if (SIZEOF_ZEND_LONG >= 8)
			n = *(zend_long *) data;
			goto _sql_long;
#else
			if (scale == 0) {
				l = slprintf(string_data, sizeof(string_data), "%" LL_MASK "d", *(ISC_INT64 *) data);
				ZVAL_STRINGL(val,string_data,l);
			} else {
				ISC_INT64 n = *(ISC_INT64 *) data, f = scales[-scale];

				if (n >= 0) {
					l = slprintf(string_data, sizeof(string_data), "%" LL_MASK "d.%0*" LL_MASK "d", n / f, -scale, n % f);
				} else if (n <= -f) {
					l = slprintf(string_data, sizeof(string_data), "%" LL_MASK "d.%0*" LL_MASK "d", n / f, -scale, -n % f);
				 } else {
					l = slprintf(string_data, sizeof(string_data), "-0.%0*" LL_MASK "d", -scale, -n % f);
				}
				ZVAL_STRINGL(val,string_data,l);
			}
			break;
#endif
		case SQL_LONG:
			n = *(ISC_LONG *) data;
		_sql_long:
			if (scale == 0) {
				ZVAL_LONG(val,n);
			} else {
				zend_long f = (zend_long) scales[-scale];

				if (n >= 0) {
					l = slprintf(string_data, sizeof(string_data), ZEND_LONG_FMT ".%0*" ZEND_LONG_FMT_SPEC, n / f, -scale,  n % f);
				} else if (n <= -f) {
					l = slprintf(string_data, sizeof(string_data), ZEND_LONG_FMT ".%0*" ZEND_LONG_FMT_SPEC, n / f, -scale,  -n % f);
				} else {
					l = slprintf(string_data, sizeof(string_data), "-0.%0*" ZEND_LONG_FMT_SPEC, -scale, -n % f);
				}
				ZVAL_STRINGL(val, string_data, l);
			}
			break;
		case SQL_FLOAT:
			ZVAL_DOUBLE(val, *(float *) data);
			break;
		case SQL_DOUBLE:
			ZVAL_DOUBLE(val, *(double *) data);
			break;
		case SQL_DATE: /* == case SQL_TIMESTAMP: */
			format = INI_STR("ibase.timestampformat");
			isc_decode_timestamp((ISC_TIMESTAMP *) data, &t);
			goto format_date_time;
		case SQL_TYPE_DATE:
			format = INI_STR("ibase.dateformat");
			isc_decode_sql_date((ISC_DATE *) data, &t);
			goto format_date_time;
		case SQL_TYPE_TIME:
			format = INI_STR("ibase.timeformat");
			isc_decode_sql_time((ISC_TIME *) data, &t);

format_date_time:
			/*
			  XXX - Might have to remove this later - seems that isc_decode_date()
			   always sets tm_isdst to 0, sometimes incorrectly (InterBase 6 bug?)
			*/
			t.tm_isdst = -1;
#if HAVE_TM_ZONE
			t.tm_zone = tzname[0];
#endif
			if (flag & PHP_IBASE_UNIXTIME) {
				ZVAL_LONG(val, mktime(&t));
			} else {
#if HAVE_STRFTIME
				l = strftime(string_data, sizeof(string_data), format, &t);
#else
				switch (type & ~1) {
					default:
						l = slprintf(string_data, sizeof(string_data), "%02d/%02d/%4d %02d:%02d:%02d", t.tm_mon+1, t.tm_mday,
							t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
						break;
					case SQL_TYPE_DATE:
						l = slprintf(string_data, sizeof(string_data), "%02d/%02d/%4d", t.tm_mon + 1, t.tm_mday, t.tm_year+1900);
						break;
					case SQL_TYPE_TIME:
						l = slprintf(string_data, sizeof(string_data), "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
						break;
				}
#endif
				ZVAL_STRINGL(val, string_data, l);
				break;
			}
	} /* switch (type) */
	return SUCCESS;
}
/* }}}	*/

static int _php_ibase_arr_zval(zval *ar_zval, char *data, zend_ulong data_size, /* {{{ */
	ibase_array *ib_array, int dim, int flag)
{
	/**
	 * Create multidimension array - recursion function
	 */
	int
		u_bound = ib_array->ar_desc.array_desc_bounds[dim].array_bound_upper,
		l_bound = ib_array->ar_desc.array_desc_bounds[dim].array_bound_lower,
		dim_len = 1 + u_bound - l_bound;
	unsigned short i;

	if (dim < ib_array->ar_desc.array_desc_dimensions) { /* array again */
		zend_ulong slice_size = data_size / dim_len;

		array_init(ar_zval);

		for (i = 0; i < dim_len; ++i) {
			zval slice_zval;

			/* recursion here */
			if (FAILURE == _php_ibase_arr_zval(&slice_zval, data, slice_size, ib_array, dim + 1,
					flag)) {
				return FAILURE;
			}
			data += slice_size;

			add_index_zval(ar_zval, l_bound + i, &slice_zval);
		}
	} else { /* data at last */

		if (FAILURE == _php_ibase_var_zval(ar_zval, data, ib_array->el_type,
				ib_array->ar_desc.array_desc_length, ib_array->ar_desc.array_desc_scale, flag)) {
			return FAILURE;
		}

		/* fix for peculiar handling of VARCHAR arrays;
		   truncate the field to the cstring length */
		if (ib_array->ar_desc.array_desc_dtype == blr_varying ||
			ib_array->ar_desc.array_desc_dtype == blr_varying2) {

			Z_STRLEN_P(ar_zval) = strlen(Z_STRVAL_P(ar_zval));
		}
	}
	return SUCCESS;
}
/* }}} */

static void _php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int fetch_type) /* {{{ */
{
	zval *result_arg;
	zend_long flag = 0;
	zend_long i, array_cnt = 0;
	ibase_result *ib_result;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &result_arg, &flag)) {
		return;
	}

	ib_result = (ibase_result *)zend_fetch_resource_ex(result_arg, LE_RESULT, le_result);

	if (ib_result->out_sqlda == NULL || !ib_result->has_more_rows) {
		RETURN_FALSE;
	}

	if (ib_result->statement_type != isc_info_sql_stmt_exec_procedure) {
		if (isc_dsql_fetch(IB_STATUS, &ib_result->stmt, 1, ib_result->out_sqlda)) {
			ib_result->has_more_rows = 0;
			if (IB_STATUS[0] && IB_STATUS[1]) { /* error in fetch */
				_php_ibase_error();
			}
			RETURN_FALSE;
		}
	} else {
		ib_result->has_more_rows = 0;
	}

	array_init(return_value);

	for (i = 0; i < ib_result->out_sqlda->sqld; ++i) {
		XSQLVAR *var = &ib_result->out_sqlda->sqlvar[i];
		char buf[METADATALENGTH+4], *alias = var->aliasname;

		if (! (fetch_type & FETCH_ROW)) {
			int i = 0;
			char const *base = "FIELD"; /* use 'FIELD' if name is empty */

			/**
			* Ensure no two columns have identical names:
			* keep generating new names until we find one that is unique.
			*/
			switch (*alias) {
				void *p;

				default:
					i = 1;
					base = alias;

					while ((p = zend_symtable_str_find_ptr(
							Z_ARRVAL_P(return_value), alias, strlen(alias))) != NULL) {

				case '\0':
						snprintf(alias = buf, sizeof(buf), "%s_%02d", base, i++);
					}
			}
		}

		if (((var->sqltype & 1) == 0) || *var->sqlind != -1) {
			zval result;

			switch (var->sqltype & ~1) {

				default:
					_php_ibase_var_zval(&result, var->sqldata, var->sqltype, var->sqllen,
						var->sqlscale, flag);
					break;
				case SQL_BLOB:
					if (flag & PHP_IBASE_FETCH_BLOBS) { /* fetch blob contents into hash */

						ibase_blob blob_handle;
						zend_ulong max_len = 0;
						static char bl_items[] = {isc_info_blob_total_length};
						char bl_info[20];
						unsigned short i;

						blob_handle.bl_handle = 0;
						blob_handle.bl_qd = *(ISC_QUAD *) var->sqldata;

						if (isc_open_blob(IB_STATUS, &ib_result->link->handle, &ib_result->trans->handle,
								&blob_handle.bl_handle, &blob_handle.bl_qd)) {
							_php_ibase_error();
							goto _php_ibase_fetch_error;
						}

						if (isc_blob_info(IB_STATUS, &blob_handle.bl_handle, sizeof(bl_items),
								bl_items, sizeof(bl_info), bl_info)) {
							_php_ibase_error();
							goto _php_ibase_fetch_error;
						}

						/* find total length of blob's data */
						for (i = 0; i < sizeof(bl_info); ) {
							unsigned short item_len;
							char item = bl_info[i++];

							if (item == isc_info_end || item == isc_info_truncated ||
								item == isc_info_error || i >= sizeof(bl_info)) {

								_php_ibase_module_error("Could not determine BLOB size (internal error)"
									);
								goto _php_ibase_fetch_error;
							}

							item_len = (unsigned short) isc_vax_integer(&bl_info[i], 2);

							if (item == isc_info_blob_total_length) {
								max_len = isc_vax_integer(&bl_info[i+2], item_len);
								break;
							}
							i += item_len+2;
						}

						if (max_len == 0) {
							ZVAL_STRING(&result, "");
						} else if (SUCCESS != _php_ibase_blob_get(&result, &blob_handle,
								max_len)) {
							goto _php_ibase_fetch_error;
						}

						if (isc_close_blob(IB_STATUS, &blob_handle.bl_handle)) {
							_php_ibase_error();
							goto _php_ibase_fetch_error;
						}

					} else { /* blob id only */
						ISC_QUAD bl_qd = *(ISC_QUAD *) var->sqldata;
						ZVAL_NEW_STR(&result, _php_ibase_quad_to_string(bl_qd));
					}
					break;
				case SQL_ARRAY:
					if (flag & PHP_IBASE_FETCH_ARRAYS) { /* array can be *huge* so only fetch if asked */
						ISC_QUAD ar_qd = *(ISC_QUAD *) var->sqldata;
						ibase_array *ib_array = &ib_result->out_array[array_cnt++];
						void *ar_data = emalloc(ib_array->ar_size);

						if (isc_array_get_slice(IB_STATUS, &ib_result->link->handle,
								&ib_result->trans->handle, &ar_qd, &ib_array->ar_desc,
								ar_data, &ib_array->ar_size)) {
							_php_ibase_error();
							efree(ar_data);
							goto _php_ibase_fetch_error;
						}

						if (FAILURE == _php_ibase_arr_zval(&result, ar_data, ib_array->ar_size, ib_array,
								0, flag)) {
							efree(ar_data);
							goto _php_ibase_fetch_error;
						}
						efree(ar_data);

					} else { /* blob id only */
						ISC_QUAD ar_qd = *(ISC_QUAD *) var->sqldata;
						ZVAL_NEW_STR(&result, _php_ibase_quad_to_string(ar_qd));
					}
					break;
				_php_ibase_fetch_error:
					zval_dtor(&result);
					RETURN_FALSE;
			} /* switch */

			if (fetch_type & FETCH_ROW) {
				add_index_zval(return_value, i, &result);
			} else {
				add_assoc_zval(return_value, alias, &result);
			}
		} else {
			if (fetch_type & FETCH_ROW) {
				add_index_null(return_value, i);
			} else {
				add_assoc_null(return_value, alias);
			}
		}
	} /* for field */
}
/* }}} */

/* {{{ proto array ibase_fetch_row(resource result [, int fetch_flags])
   Fetch a row  from the results of a query */
PHP_FUNCTION(ibase_fetch_row)
{
	_php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FETCH_ROW);
}
/* }}} */

/* {{{ proto array ibase_fetch_assoc(resource result [, int fetch_flags])
   Fetch a row  from the results of a query */
PHP_FUNCTION(ibase_fetch_assoc)
{
	_php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FETCH_ARRAY);
}
/* }}} */

/* {{{ proto object ibase_fetch_object(resource result [, int fetch_flags])
   Fetch a object from the results of a query */
PHP_FUNCTION(ibase_fetch_object)
{
	_php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FETCH_ARRAY);

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		convert_to_object(return_value);
	}
}
/* }}} */


/* {{{ proto bool ibase_name_result(resource result, string name)
   Assign a name to a result for use with ... WHERE CURRENT OF <name> statements */
PHP_FUNCTION(ibase_name_result)
{
	zval *result_arg;
	char *name_arg;
	size_t name_arg_len;
	ibase_result *ib_result;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &result_arg, &name_arg, &name_arg_len) == FAILURE) {
		return;
	}

	ib_result = (ibase_result *)zend_fetch_resource_ex(result_arg, LE_RESULT, le_result);

	if (isc_dsql_set_cursor_name(IB_STATUS, &ib_result->stmt, name_arg, 0)) {
		_php_ibase_error();
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool ibase_free_result(resource result)
   Free the memory used by a result */
PHP_FUNCTION(ibase_free_result)
{
	zval *result_arg;
	ibase_result *ib_result;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result_arg) == FAILURE) {
		return;
	}

	ib_result = (ibase_result *)zend_fetch_resource_ex(result_arg, LE_RESULT, le_result);
	zend_list_delete(Z_RES_P(result_arg));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource ibase_prepare(resource link_identifier[, string query [, resource trans_identifier ]])
   Prepare a query for later execution */
PHP_FUNCTION(ibase_prepare)
{
	zval *link_arg, *trans_arg;
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	size_t query_len;
	zend_resource *trans_res = NULL;
	ibase_query *ib_query;
	char *query;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &query, &query_len) == FAILURE) {
			return;
		}
		ib_link = (ibase_db_link *)zend_fetch_resource2(IBG(default_link), LE_LINK, le_link, le_plink);
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &link_arg, &query, &query_len) == FAILURE) {
			return;
		}
		_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, link_arg, &ib_link, &trans);

		if (trans != NULL) {
			trans_res = Z_RES_P(link_arg);
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rrs", &link_arg, &trans_arg, &query, &query_len) == FAILURE) {
			return;
		}
		ib_link = (ibase_db_link *)zend_fetch_resource2_ex(link_arg, LE_LINK, le_link, le_plink);
		trans = (ibase_trans *)zend_fetch_resource_ex(trans_arg, LE_TRANS, le_trans);
		trans_res = Z_RES_P(trans_arg);
	}

	if (FAILURE == _php_ibase_def_trans(ib_link, &trans)) {
		RETURN_FALSE;
	}

	ib_query = (ibase_query *) emalloc(sizeof(ibase_query));

	if (FAILURE == _php_ibase_alloc_query(ib_query, ib_link, trans, query, ib_link->dialect, trans_res)) {
		efree(ib_query);
		RETURN_FALSE;
	}
	RETVAL_RES(zend_register_resource(ib_query, le_query));
	Z_TRY_ADDREF_P(return_value);
}
/* }}} */

/* {{{ proto mixed ibase_execute(resource query [, mixed bind_arg [, mixed bind_arg [, ...]]])
   Execute a previously prepared query */
PHP_FUNCTION(ibase_execute)
{
	zval *query, *args = NULL;
	ibase_query *ib_query;
	ibase_result *result = NULL;
	int bind_n = 0;

	RESET_ERRMSG;

	RETVAL_FALSE;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|r*", &query, &args, &bind_n)) {
		return;
	}

	ib_query = (ibase_query *)zend_fetch_resource_ex(query, LE_QUERY, le_query);

	do {
		int	expected_n = ib_query->in_sqlda ? ib_query->in_sqlda->sqld : 0;

		if (bind_n != expected_n) {
			php_error_docref(NULL, (bind_n < expected_n) ? E_WARNING : E_NOTICE,
				"Statement expects %d arguments, %d given", expected_n, bind_n);

			if (bind_n < expected_n) {
				break;
			}
		}

		/* Have we used this cursor before and it's still open (exec proc has no cursor) ? */
		if (ib_query->result_res != NULL
				&& ib_query->statement_type != isc_info_sql_stmt_exec_procedure) {
			IBDEBUG("Implicitly closing a cursor");

			if (isc_dsql_free_statement(IB_STATUS, &ib_query->stmt, DSQL_close)) {
				_php_ibase_error();
				break;
			}
			zend_list_delete(ib_query->result_res);
			ib_query->result_res = NULL;
		}

		if (FAILURE == _php_ibase_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, &result, ib_query,
				args)) {
		    break;
		}

		/* free the query if trans handle was released */
		if (ib_query->trans->handle == 0) {
			zend_list_delete(Z_RES_P(query));
		}

		if (result != NULL) {
			zval *ret;

			result->type = EXECUTE_RESULT;
			if (ib_query->statement_type == isc_info_sql_stmt_exec_procedure) {
				result->stmt = 0;
			}

			ret = zend_list_insert(result, le_result);
			ib_query->result_res = Z_RES_P(ret);
			ZVAL_COPY_VALUE(return_value, ret);
			Z_TRY_ADDREF_P(return_value);
			Z_TRY_ADDREF_P(return_value);
		}
	} while (0);
}
/* }}} */

/* {{{ proto bool ibase_free_query(resource query)
   Free memory used by a query */
PHP_FUNCTION(ibase_free_query)
{
	zval *query_arg;
	ibase_query *ib_query;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &query_arg) == FAILURE) {
		return;
	}

	ib_query = (ibase_query *)zend_fetch_resource_ex(query_arg, LE_QUERY, le_query);
	if (!ib_query) {
		RETURN_FALSE;
	}

	zend_list_close(Z_RES_P(query_arg));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ibase_num_fields(resource query_result)
   Get the number of fields in result */
PHP_FUNCTION(ibase_num_fields)
{
	zval *result;
	int type;
	XSQLDA *sqlda;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result) == FAILURE) {
		return;
	}

	type = Z_RES_P(result)->type;

	if (type == le_query) {
		ibase_query *ib_query;

		ib_query = (ibase_query *)zend_fetch_resource_ex(result, LE_QUERY, le_query);
		sqlda = ib_query->out_sqlda;
	} else {
		ibase_result *ib_result;

		ib_result = (ibase_result *)zend_fetch_resource_ex(result, LE_RESULT, le_result);
		sqlda = ib_result->out_sqlda;
	}

	if (sqlda == NULL) {
		RETURN_LONG(0);
	} else {
		RETURN_LONG(sqlda->sqld);
	}
}
/* }}} */

static void _php_ibase_field_info(zval *return_value, XSQLVAR *var) /* {{{ */
{
	unsigned short len;
	char buf[16], *s = buf;

	array_init(return_value);

	add_index_stringl(return_value, 0, var->sqlname, var->sqlname_length);
	add_assoc_stringl(return_value, "name", var->sqlname, var->sqlname_length);

	add_index_stringl(return_value, 1, var->aliasname, var->aliasname_length);
	add_assoc_stringl(return_value, "alias", var->aliasname, var->aliasname_length);

	add_index_stringl(return_value, 2, var->relname, var->relname_length);
	add_assoc_stringl(return_value, "relation", var->relname, var->relname_length);

	len = slprintf(buf, 16, "%d", var->sqllen);
	add_index_stringl(return_value, 3, buf, len);
	add_assoc_stringl(return_value, "length", buf, len);

	if (var->sqlscale < 0) {
		unsigned short precision = 0;

		switch (var->sqltype & ~1) {

			case SQL_SHORT:
				precision = 4;
				break;
			case SQL_LONG:
				precision = 9;
				break;
			case SQL_INT64:
				precision = 18;
				break;
		}
		len = slprintf(buf, 16, "NUMERIC(%d,%d)", precision, -var->sqlscale);
		add_index_stringl(return_value, 4, s, len);
		add_assoc_stringl(return_value, "type", s, len);
	} else {
		switch (var->sqltype & ~1) {
			case SQL_TEXT:
				s = "CHAR";
				break;
			case SQL_VARYING:
				s = "VARCHAR";
				break;
			case SQL_SHORT:
				s = "SMALLINT";
				break;
			case SQL_LONG:
				s = "INTEGER";
				break;
			case SQL_FLOAT:
				s = "FLOAT"; break;
			case SQL_DOUBLE:
			case SQL_D_FLOAT:
				s = "DOUBLE PRECISION"; break;
			case SQL_INT64:
				s = "BIGINT";
				break;
			case SQL_TIMESTAMP:
				s = "TIMESTAMP";
				break;
			case SQL_TYPE_DATE:
				s = "DATE";
				break;
			case SQL_TYPE_TIME:
				s = "TIME";
				break;
			case SQL_BLOB:
				s = "BLOB";
				break;
			case SQL_ARRAY:
				s = "ARRAY";
				break;
				/* FIXME: provide more detailed information about the field type, field size
				 * and array dimensions */
			case SQL_QUAD:
				s = "QUAD";
				break;
		}
		add_index_string(return_value, 4, s);
		add_assoc_string(return_value, "type", s);
	}
}
/* }}} */

/* {{{ proto array ibase_field_info(resource query_result, int field_number)
   Get information about a field */
PHP_FUNCTION(ibase_field_info)
{
	zval *result_arg;
	zend_long field_arg;
	int type;
	XSQLDA *sqlda;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &result_arg, &field_arg) == FAILURE) {
		return;
	}

	type = Z_RES_P(result_arg)->type;

	if (type == le_query) {
		ibase_query *ib_query;

		ib_query= (ibase_query *)zend_fetch_resource_ex(result_arg, LE_QUERY, le_query);
		sqlda = ib_query->out_sqlda;
	} else {
		ibase_result *ib_result;

		ib_result = (ibase_result *)zend_fetch_resource_ex(result_arg, LE_RESULT, le_result);
		sqlda = ib_result->out_sqlda;
	}

	if (sqlda == NULL) {
		_php_ibase_module_error("Trying to get field info from a non-select query");
		RETURN_FALSE;
	}

	if (field_arg < 0 || field_arg >= sqlda->sqld) {
		RETURN_FALSE;
	}
	_php_ibase_field_info(return_value, sqlda->sqlvar + field_arg);
}
/* }}} */

/* {{{ proto int ibase_num_params(resource query)
   Get the number of params in a prepared query */
PHP_FUNCTION(ibase_num_params)
{
	zval *result;
	ibase_query *ib_query;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result) == FAILURE) {
		return;
	}

	ib_query = (ibase_query *)zend_fetch_resource_ex(result, LE_QUERY, le_query);

	if (ib_query->in_sqlda == NULL) {
		RETURN_LONG(0);
	} else {
		RETURN_LONG(ib_query->in_sqlda->sqld);
	}
}
/* }}} */

/* {{{ proto array ibase_param_info(resource query, int field_number)
   Get information about a parameter */
PHP_FUNCTION(ibase_param_info)
{
	zval *result_arg;
	zend_long field_arg;
	ibase_query *ib_query;

	RESET_ERRMSG;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &result_arg, &field_arg) == FAILURE) {
		return;
	}

	ib_query = (ibase_query *)zend_fetch_resource_ex(result_arg, LE_QUERY, le_query);

	if (ib_query->in_sqlda == NULL) {
		RETURN_FALSE;
	}

	if (field_arg < 0 || field_arg >= ib_query->in_sqlda->sqld) {
		RETURN_FALSE;
	}

	_php_ibase_field_info(return_value,ib_query->in_sqlda->sqlvar + field_arg);
}
/* }}} */

#endif /* HAVE_IBASE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
