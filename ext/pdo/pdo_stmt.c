/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
  |         Marcus Boerger <helly@php.net>                               |
  |         Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* The PDO Statement Handle Class */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "php_pdo.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "php_memory_streams.h"
#include "pdo_stmt_arginfo.h"

#define PHP_STMT_GET_OBJ	\
  pdo_stmt_t *stmt = Z_PDO_STMT_P(ZEND_THIS);	\
  if (!stmt->dbh) {	\
	  RETURN_FALSE;	\
  }	\

static inline int rewrite_name_to_position(pdo_stmt_t *stmt, struct pdo_bound_param_data *param) /* {{{ */
{
	if (stmt->bound_param_map) {
		/* rewriting :name to ? style.
		 * We need to fixup the parameter numbers on the parameters.
		 * If we find that a given named parameter has been used twice,
		 * we will raise an error, as we can't be sure that it is safe
		 * to bind multiple parameters onto the same zval in the underlying
		 * driver */
		char *name;
		int position = 0;

		if (stmt->named_rewrite_template) {
			/* this is not an error here */
			return 1;
		}
		if (!param->name) {
			/* do the reverse; map the parameter number to the name */
			if ((name = zend_hash_index_find_ptr(stmt->bound_param_map, param->paramno)) != NULL) {
				param->name = zend_string_init(name, strlen(name), 0);
				return 1;
			}
			pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "parameter was not defined");
			return 0;
		}

		ZEND_HASH_FOREACH_PTR(stmt->bound_param_map, name) {
			if (strncmp(name, ZSTR_VAL(param->name), ZSTR_LEN(param->name) + 1)) {
				position++;
				continue;
			}
			if (param->paramno >= 0) {
				pdo_raise_impl_error(stmt->dbh, stmt, "IM001", "PDO refuses to handle repeating the same :named parameter for multiple positions with this driver, as it might be unsafe to do so.  Consider using a separate name for each parameter instead");
				return -1;
			}
			param->paramno = position;
			return 1;
		} ZEND_HASH_FOREACH_END();
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "parameter was not defined");
		return 0;
	}
	return 1;
}
/* }}} */

/* trigger callback hook for parameters */
static int dispatch_param_event(pdo_stmt_t *stmt, enum pdo_param_event event_type) /* {{{ */
{
	int ret = 1, is_param = 1;
	struct pdo_bound_param_data *param;
	HashTable *ht;

	if (stmt->dbh->skip_param_evt & (1 << event_type)) {
		return 1;
	}

	if (!stmt->methods->param_hook) {
		return 1;
	}

	ht = stmt->bound_params;

iterate:
	if (ht) {
		ZEND_HASH_FOREACH_PTR(ht, param) {
			if (!stmt->methods->param_hook(stmt, param, event_type)) {
				ret = 0;
				break;
			}
		} ZEND_HASH_FOREACH_END();
	}
	if (ret && is_param) {
		ht = stmt->bound_columns;
		is_param = 0;
		goto iterate;
	}

	return ret;
}
/* }}} */

int pdo_stmt_describe_columns(pdo_stmt_t *stmt) /* {{{ */
{
	int col;

	stmt->columns = ecalloc(stmt->column_count, sizeof(struct pdo_column_data));

	for (col = 0; col < stmt->column_count; col++) {
		if (!stmt->methods->describer(stmt, col)) {
			return 0;
		}

		/* if we are applying case conversions on column names, do so now */
		if (stmt->dbh->native_case != stmt->dbh->desired_case && stmt->dbh->desired_case != PDO_CASE_NATURAL) {
			char *s = ZSTR_VAL(stmt->columns[col].name);

			switch (stmt->dbh->desired_case) {
				case PDO_CASE_UPPER:
					while (*s != '\0') {
						*s = toupper(*s);
						s++;
					}
					break;
				case PDO_CASE_LOWER:
					while (*s != '\0') {
						*s = tolower(*s);
						s++;
					}
					break;
				default:
					;
			}
		}

		/* update the column index on named bound parameters */
		if (stmt->bound_columns) {
			struct pdo_bound_param_data *param;

			if ((param = zend_hash_find_ptr(stmt->bound_columns,
					stmt->columns[col].name)) != NULL) {
				param->paramno = col;
			}
		}

	}
	return 1;
}
/* }}} */

static void get_lazy_object(pdo_stmt_t *stmt, zval *return_value) /* {{{ */
{
	if (Z_ISUNDEF(stmt->lazy_object_ref)) {
		pdo_row_t *row = ecalloc(1, sizeof(pdo_row_t));
		row->stmt = stmt;
		zend_object_std_init(&row->std, pdo_row_ce);
		ZVAL_OBJ(&stmt->lazy_object_ref, &row->std);
		row->std.handlers = &pdo_row_object_handlers;
		GC_ADDREF(&stmt->std);
		GC_DELREF(&row->std);
	}
	ZVAL_COPY(return_value, &stmt->lazy_object_ref);
}
/* }}} */

static void param_dtor(zval *el) /* {{{ */
{
	struct pdo_bound_param_data *param = (struct pdo_bound_param_data *)Z_PTR_P(el);

	/* tell the driver that it is going away */
	if (param->stmt->methods->param_hook) {
		param->stmt->methods->param_hook(param->stmt, param, PDO_PARAM_EVT_FREE);
	}

	if (param->name) {
		zend_string_release_ex(param->name, 0);
	}

	if (!Z_ISUNDEF(param->parameter)) {
		zval_ptr_dtor(&param->parameter);
		ZVAL_UNDEF(&param->parameter);
	}
	if (!Z_ISUNDEF(param->driver_params)) {
		zval_ptr_dtor(&param->driver_params);
	}
	efree(param);
}
/* }}} */

static int really_register_bound_param(struct pdo_bound_param_data *param, pdo_stmt_t *stmt, int is_param) /* {{{ */
{
	HashTable *hash;
	zval *parameter;
	struct pdo_bound_param_data *pparam = NULL;

	hash = is_param ? stmt->bound_params : stmt->bound_columns;

	if (!hash) {
		ALLOC_HASHTABLE(hash);
		zend_hash_init(hash, 13, NULL, param_dtor, 0);

		if (is_param) {
			stmt->bound_params = hash;
		} else {
			stmt->bound_columns = hash;
		}
	}

	if (!Z_ISREF(param->parameter)) {
		parameter = &param->parameter;
	} else {
		parameter = Z_REFVAL(param->parameter);
	}

	if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_STR && param->max_value_len <= 0 && !Z_ISNULL_P(parameter)) {
		if (!try_convert_to_string(parameter)) {
			return 0;
		}
	} else if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_INT && (Z_TYPE_P(parameter) == IS_FALSE || Z_TYPE_P(parameter) == IS_TRUE)) {
		convert_to_long(parameter);
	} else if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_BOOL && Z_TYPE_P(parameter) == IS_LONG) {
		convert_to_boolean(parameter);
	}

	param->stmt = stmt;
	param->is_param = is_param;

	if (Z_REFCOUNTED(param->driver_params)) {
		Z_ADDREF(param->driver_params);
	}

	if (!is_param && param->name && stmt->columns) {
		/* try to map the name to the column */
		int i;

		for (i = 0; i < stmt->column_count; i++) {
			if (ZSTR_LEN(stmt->columns[i].name) == ZSTR_LEN(param->name) &&
			    strncmp(ZSTR_VAL(stmt->columns[i].name), ZSTR_VAL(param->name), ZSTR_LEN(param->name) + 1) == 0) {
				param->paramno = i;
				break;
			}
		}

		/* if you prepare and then execute passing an array of params keyed by names,
		 * then this will trigger, and we don't want that */
		if (param->paramno == -1) {
			char *tmp;
			spprintf(&tmp, 0, "Did not find column name '%s' in the defined columns; it will not be bound", ZSTR_VAL(param->name));
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", tmp);
			efree(tmp);
		}
	}

	if (param->name) {
		if (is_param && ZSTR_VAL(param->name)[0] != ':') {
			zend_string *temp = zend_string_alloc(ZSTR_LEN(param->name) + 1, 0);
			ZSTR_VAL(temp)[0] = ':';
			memmove(ZSTR_VAL(temp) + 1, ZSTR_VAL(param->name), ZSTR_LEN(param->name) + 1);
			param->name = temp;
		} else {
			param->name = zend_string_init(ZSTR_VAL(param->name), ZSTR_LEN(param->name), 0);
		}
	}

	if (is_param && !rewrite_name_to_position(stmt, param)) {
		if (param->name) {
			zend_string_release_ex(param->name, 0);
			param->name = NULL;
		}
		return 0;
	}

	/* ask the driver to perform any normalization it needs on the
	 * parameter name.  Note that it is illegal for the driver to take
	 * a reference to param, as it resides in transient storage only
	 * at this time. */
	if (stmt->methods->param_hook) {
		if (!stmt->methods->param_hook(stmt, param, PDO_PARAM_EVT_NORMALIZE
				)) {
			if (param->name) {
				zend_string_release_ex(param->name, 0);
				param->name = NULL;
			}
			return 0;
		}
	}

	/* delete any other parameter registered with this number.
	 * If the parameter is named, it will be removed and correctly
	 * disposed of by the hash_update call that follows */
	if (param->paramno >= 0) {
		zend_hash_index_del(hash, param->paramno);
	}

	/* allocate storage for the parameter, keyed by its "canonical" name */
	if (param->name) {
		pparam = zend_hash_update_mem(hash, param->name, param, sizeof(struct pdo_bound_param_data));
	} else {
		pparam = zend_hash_index_update_mem(hash, param->paramno, param, sizeof(struct pdo_bound_param_data));
	}

	/* tell the driver we just created a parameter */
	if (stmt->methods->param_hook) {
		if (!stmt->methods->param_hook(stmt, pparam, PDO_PARAM_EVT_ALLOC
					)) {
			/* undo storage allocation; the hash will free the parameter
			 * name if required */
			if (pparam->name) {
				zend_hash_del(hash, pparam->name);
			} else {
				zend_hash_index_del(hash, pparam->paramno);
			}
			/* param->parameter is freed by hash dtor */
			ZVAL_UNDEF(&param->parameter);
			return 0;
		}
	}
	return 1;
}
/* }}} */

/* {{{ Execute a prepared statement, optionally binding parameters */
PHP_METHOD(PDOStatement, execute)
{
	zval *input_params = NULL;
	int ret = 1;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(input_params)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	PDO_STMT_CLEAR_ERR();

	if (input_params) {
		struct pdo_bound_param_data param;
		zval *tmp;
		zend_string *key = NULL;
		zend_ulong num_index;

		if (stmt->bound_params) {
			zend_hash_destroy(stmt->bound_params);
			FREE_HASHTABLE(stmt->bound_params);
			stmt->bound_params = NULL;
		}

		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(input_params), num_index, key, tmp) {
			memset(&param, 0, sizeof(param));

			if (key) {
				/* yes this is correct.  we don't want to count the null byte.  ask wez */
				param.name = key;
				param.paramno = -1;
			} else {
				/* we're okay to be zero based here */
				/* num_index is unsignend
				if (num_index < 0) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY093", NULL);
					RETURN_FALSE;
				}
				*/
				param.paramno = num_index;
			}

			param.param_type = PDO_PARAM_STR;
			ZVAL_COPY(&param.parameter, tmp);

			if (!really_register_bound_param(&param, stmt, 1)) {
				if (!Z_ISUNDEF(param.parameter)) {
					zval_ptr_dtor(&param.parameter);
				}
				RETURN_FALSE;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (PDO_PLACEHOLDER_NONE == stmt->supports_placeholders) {
		/* handle the emulated parameter binding,
         * stmt->active_query_string holds the query with binds expanded and
		 * quoted.
         */

		/* string is leftover from previous calls so PDOStatement::debugDumpParams() can access */
		if (stmt->active_query_string && stmt->active_query_string != stmt->query_string) {
			efree(stmt->active_query_string);
		}
		stmt->active_query_string = NULL;

		ret = pdo_parse_params(stmt, stmt->query_string, stmt->query_stringlen,
			&stmt->active_query_string, &stmt->active_query_stringlen);

		if (ret == 0) {
			/* no changes were made */
			stmt->active_query_string = stmt->query_string;
			stmt->active_query_stringlen = stmt->query_stringlen;
			ret = 1;
		} else if (ret == -1) {
			/* something broke */
			PDO_HANDLE_STMT_ERR();
			RETURN_FALSE;
		}
	} else if (!dispatch_param_event(stmt, PDO_PARAM_EVT_EXEC_PRE)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}
	if (stmt->methods->executer(stmt)) {
		if (!stmt->executed) {
			/* this is the first execute */

			if (stmt->dbh->alloc_own_columns && !stmt->columns) {
				/* for "big boy" drivers, we need to allocate memory to fetch
				 * the results into, so lets do that now */
				ret = pdo_stmt_describe_columns(stmt);
			}

			stmt->executed = 1;
		}

		if (ret && !dispatch_param_event(stmt, PDO_PARAM_EVT_EXEC_POST)) {
			RETURN_FALSE;
		}

		RETURN_BOOL(ret);
	}
	PDO_HANDLE_STMT_ERR();
	RETURN_FALSE;
}
/* }}} */

static inline void fetch_value(pdo_stmt_t *stmt, zval *dest, int colno, int *type_override) /* {{{ */
{
	struct pdo_column_data *col;
	char *value = NULL;
	size_t value_len = 0;
	int caller_frees = 0;
	int type, new_type;

	if (colno < 0 || colno >= stmt->column_count) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "Invalid column index");
		ZVAL_FALSE(dest);

		return;
	}

	col = &stmt->columns[colno];
	type = PDO_PARAM_TYPE(col->param_type);
	new_type =  type_override ? (int)PDO_PARAM_TYPE(*type_override) : type;

	value = NULL;
	value_len = 0;

	stmt->methods->get_col(stmt, colno, &value, &value_len, &caller_frees);

	switch (type) {
		case PDO_PARAM_ZVAL:
			if (value && value_len == sizeof(zval)) {
				ZVAL_COPY_VALUE(dest, (zval *)value);
			} else {
				ZVAL_NULL(dest);
			}

			if (Z_TYPE_P(dest) == IS_NULL) {
				type = new_type;
			}
			break;

		case PDO_PARAM_INT:
			if (value && value_len == sizeof(zend_long)) {
				ZVAL_LONG(dest, *(zend_long*)value);
				break;
			}
			ZVAL_NULL(dest);
			break;

		case PDO_PARAM_BOOL:
			if (value && value_len == sizeof(zend_bool)) {
				ZVAL_BOOL(dest, *(zend_bool*)value);
				break;
			}
			ZVAL_NULL(dest);
			break;

		case PDO_PARAM_LOB:
			if (value == NULL) {
				ZVAL_NULL(dest);
			} else if (value_len == 0) {
				/* Warning, empty strings need to be passed as stream */
				if (stmt->dbh->stringify || new_type == PDO_PARAM_STR) {
					zend_string *buf;
					buf = php_stream_copy_to_mem((php_stream*)value, PHP_STREAM_COPY_ALL, 0);
					if (buf == NULL) {
						ZVAL_EMPTY_STRING(dest);
					} else {
						ZVAL_STR(dest, buf);
					}
					php_stream_close((php_stream*)value);
				} else {
					php_stream_to_zval((php_stream*)value, dest);
				}
			} else if (!stmt->dbh->stringify && new_type != PDO_PARAM_STR) {
				/* they gave us a string, but LOBs are represented as streams in PDO */
				php_stream *stm;
#ifdef TEMP_STREAM_TAKE_BUFFER
				if (caller_frees) {
					stm = php_stream_memory_open(TEMP_STREAM_TAKE_BUFFER, value, value_len);
					if (stm) {
						caller_frees = 0;
					}
				} else
#endif
				{
					stm = php_stream_memory_open(TEMP_STREAM_READONLY, value, value_len);
				}
				if (stm) {
					php_stream_to_zval(stm, dest);
				} else {
					ZVAL_NULL(dest);
				}
			} else {
				ZVAL_STRINGL(dest, value, value_len);
			}
			break;

		case PDO_PARAM_STR:
			if (value && !(value_len == 0 && stmt->dbh->oracle_nulls == PDO_NULL_EMPTY_STRING)) {
				ZVAL_STRINGL(dest, value, value_len);
				break;
			}
		default:
			ZVAL_NULL(dest);
	}

	if (type != new_type) {
		switch (new_type) {
			case PDO_PARAM_INT:
				convert_to_long_ex(dest);
				break;
			case PDO_PARAM_BOOL:
				convert_to_boolean_ex(dest);
				break;
			case PDO_PARAM_STR:
				convert_to_string_ex(dest);
				break;
			case PDO_PARAM_NULL:
				convert_to_null_ex(dest);
				break;
			default:
				;
		}
	}

	if (caller_frees && value) {
		efree(value);
	}

	if (stmt->dbh->stringify) {
		switch (Z_TYPE_P(dest)) {
			case IS_LONG:
			case IS_DOUBLE:
				convert_to_string(dest);
				break;
		}
	}

	if (Z_TYPE_P(dest) == IS_NULL && stmt->dbh->oracle_nulls == PDO_NULL_TO_STRING) {
		ZVAL_EMPTY_STRING(dest);
	}
}
/* }}} */

static int do_fetch_common(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori, zend_long offset, int do_bind) /* {{{ */
{
	if (!stmt->executed) {
		return 0;
	}

	if (!dispatch_param_event(stmt, PDO_PARAM_EVT_FETCH_PRE)) {
		return 0;
	}

	if (!stmt->methods->fetcher(stmt, ori, offset)) {
		return 0;
	}

	/* some drivers might need to describe the columns now */
	if (!stmt->columns && !pdo_stmt_describe_columns(stmt)) {
		return 0;
	}

	if (!dispatch_param_event(stmt, PDO_PARAM_EVT_FETCH_POST)) {
		return 0;
	}

	if (do_bind && stmt->bound_columns) {
		/* update those bound column variables now */
		struct pdo_bound_param_data *param;

		ZEND_HASH_FOREACH_PTR(stmt->bound_columns, param) {
			if (param->paramno >= 0) {
				if (!Z_ISREF(param->parameter)) {
					continue;
				}

				/* delete old value */
				zval_ptr_dtor(Z_REFVAL(param->parameter));

				/* set new value */
				fetch_value(stmt, Z_REFVAL(param->parameter), param->paramno, (int *)&param->param_type);

				/* TODO: some smart thing that avoids duplicating the value in the
				 * general loop below.  For now, if you're binding output columns,
				 * it's better to use LAZY or BOUND fetches if you want to shave
				 * off those cycles */
			}
		} ZEND_HASH_FOREACH_END();
	}

	return 1;
}
/* }}} */

static int do_fetch_class_prepare(pdo_stmt_t *stmt) /* {{{ */
{
	zend_class_entry *ce = stmt->fetch.cls.ce;
	zend_fcall_info *fci = &stmt->fetch.cls.fci;
	zend_fcall_info_cache *fcc = &stmt->fetch.cls.fcc;

	fci->size = sizeof(zend_fcall_info);

	if (!ce) {
		stmt->fetch.cls.ce = ZEND_STANDARD_CLASS_DEF_PTR;
		ce = ZEND_STANDARD_CLASS_DEF_PTR;
	}

	if (ce->constructor) {
		ZVAL_UNDEF(&fci->function_name);
		fci->retval = &stmt->fetch.cls.retval;
		fci->param_count = 0;
		fci->params = NULL;

		zend_fcall_info_args_ex(fci, ce->constructor, &stmt->fetch.cls.ctor_args);

		fcc->function_handler = ce->constructor;
		fcc->called_scope = ce;
		return 1;
	} else if (!Z_ISUNDEF(stmt->fetch.cls.ctor_args)) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "user-supplied class does not have a constructor, use NULL for the ctor_params parameter, or simply omit it");
		return 0;
	} else {
		return 1; /* no ctor no args is also ok */
	}
}
/* }}} */

static int make_callable_ex(pdo_stmt_t *stmt, zval *callable, zend_fcall_info * fci, zend_fcall_info_cache * fcc, int num_args) /* {{{ */
{
	char *is_callable_error = NULL;

	if (zend_fcall_info_init(callable, 0, fci, fcc, NULL, &is_callable_error) == FAILURE) {
		if (is_callable_error) {
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", is_callable_error);
			efree(is_callable_error);
		} else {
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "user-supplied function must be a valid callback");
		}
		return 0;
	}
	if (is_callable_error) {
		/* Possible error message */
		efree(is_callable_error);
	}

	fci->param_count = num_args; /* probably less */
	fci->params = safe_emalloc(sizeof(zval), num_args, 0);

	return 1;
}
/* }}} */

static int do_fetch_func_prepare(pdo_stmt_t *stmt) /* {{{ */
{
	zend_fcall_info *fci = &stmt->fetch.cls.fci;
	zend_fcall_info_cache *fcc = &stmt->fetch.cls.fcc;

	if (!make_callable_ex(stmt, &stmt->fetch.func.function, fci, fcc, stmt->column_count)) {
		return 0;
	} else {
		stmt->fetch.func.values = safe_emalloc(sizeof(zval), stmt->column_count, 0);
		return 1;
	}
}
/* }}} */

static void do_fetch_opt_finish(pdo_stmt_t *stmt, int free_ctor_agrs) /* {{{ */
{
	/* fci.size is used to check if it is valid */
	if (stmt->fetch.cls.fci.size && stmt->fetch.cls.fci.params) {
		if (!Z_ISUNDEF(stmt->fetch.cls.ctor_args)) {
		    /* Added to free constructor arguments */
			zend_fcall_info_args_clear(&stmt->fetch.cls.fci, 1);
		} else {
			efree(stmt->fetch.cls.fci.params);
		}
		stmt->fetch.cls.fci.params = NULL;
	}

	stmt->fetch.cls.fci.size = 0;
	if (!Z_ISUNDEF(stmt->fetch.cls.ctor_args) && free_ctor_agrs) {
		zval_ptr_dtor(&stmt->fetch.cls.ctor_args);
		ZVAL_UNDEF(&stmt->fetch.cls.ctor_args);
		stmt->fetch.cls.fci.param_count = 0;
	}
	if (stmt->fetch.func.values) {
		efree(stmt->fetch.func.values);
		stmt->fetch.func.values = NULL;
	}
}
/* }}} */

/* perform a fetch.  If do_bind is true, update any bound columns.
 * If return_value is not null, store values into it according to HOW. */
static int do_fetch(pdo_stmt_t *stmt, int do_bind, zval *return_value, enum pdo_fetch_type how, enum pdo_fetch_orientation ori, zend_long offset, zval *return_all) /* {{{ */
{
	int flags, idx, old_arg_count = 0;
	zend_class_entry *ce = NULL, *old_ce = NULL;
	zval grp_val, *pgrp, retval, old_ctor_args = {{0}, {0}, {0}};
	int colno;

	if (how == PDO_FETCH_USE_DEFAULT) {
		how = stmt->default_fetch_type;
	}
	flags = how & PDO_FETCH_FLAGS;
	how = how & ~PDO_FETCH_FLAGS;

	if (!do_fetch_common(stmt, ori, offset, do_bind)) {
		return 0;
	}

	if (how == PDO_FETCH_BOUND) {
		RETVAL_TRUE;
		return 1;
	}

	if ((flags & PDO_FETCH_GROUP) && stmt->fetch.column == -1) {
		colno = 1;
	} else {
		colno = stmt->fetch.column;
	}

	if (return_value) {
		int i = 0;

		if (how == PDO_FETCH_LAZY) {
			get_lazy_object(stmt, return_value);
			return 1;
		}

		RETVAL_FALSE;

		switch (how) {
			case PDO_FETCH_USE_DEFAULT:
			case PDO_FETCH_ASSOC:
			case PDO_FETCH_BOTH:
			case PDO_FETCH_NUM:
			case PDO_FETCH_NAMED:
				if (!return_all) {
					array_init_size(return_value, stmt->column_count);
				} else {
					array_init(return_value);
				}
				break;

			case PDO_FETCH_KEY_PAIR:
				if (stmt->column_count != 2) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO::FETCH_KEY_PAIR fetch mode requires the result set to contain exactly 2 columns.");
					return 0;
				}
				if (!return_all) {
					array_init(return_value);
				}
				break;

			case PDO_FETCH_COLUMN:
				if (colno >= 0 && colno < stmt->column_count) {
					if (flags == PDO_FETCH_GROUP && stmt->fetch.column == -1) {
						fetch_value(stmt, return_value, 1, NULL);
					} else if (flags == PDO_FETCH_GROUP && colno) {
						fetch_value(stmt, return_value, 0, NULL);
					} else {
						fetch_value(stmt, return_value, colno, NULL);
					}
					if (!return_all) {
						return 1;
					} else {
						break;
					}
				} else {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "Invalid column index");
				}
				return 0;

			case PDO_FETCH_OBJ:
				object_init_ex(return_value, ZEND_STANDARD_CLASS_DEF_PTR);
				break;

			case PDO_FETCH_CLASS:
				if (flags & PDO_FETCH_CLASSTYPE) {
					zval val;
					zend_class_entry *cep;

					old_ce = stmt->fetch.cls.ce;
					ZVAL_COPY_VALUE(&old_ctor_args, &stmt->fetch.cls.ctor_args);
					old_arg_count = stmt->fetch.cls.fci.param_count;
					do_fetch_opt_finish(stmt, 0);

					fetch_value(stmt, &val, i++, NULL);
					if (Z_TYPE(val) != IS_NULL) {
						if (!try_convert_to_string(&val)) {
							return 0;
						}
						if ((cep = zend_lookup_class(Z_STR(val))) == NULL) {
							stmt->fetch.cls.ce = ZEND_STANDARD_CLASS_DEF_PTR;
						} else {
							stmt->fetch.cls.ce = cep;
						}
					}

					do_fetch_class_prepare(stmt);
					zval_ptr_dtor_str(&val);
				}
				ce = stmt->fetch.cls.ce;
				if (!ce) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "No fetch class specified");
					return 0;
				}
				if ((flags & PDO_FETCH_SERIALIZE) == 0) {
					if (UNEXPECTED(object_init_ex(return_value, ce) != SUCCESS)) {
						return 0;
					}
					if (!stmt->fetch.cls.fci.size) {
						if (!do_fetch_class_prepare(stmt))
						{
							return 0;
						}
					}
					if (ce->constructor && (flags & PDO_FETCH_PROPS_LATE)) {
						stmt->fetch.cls.fci.object = Z_OBJ_P(return_value);
						stmt->fetch.cls.fcc.object = Z_OBJ_P(return_value);
						if (zend_call_function(&stmt->fetch.cls.fci, &stmt->fetch.cls.fcc) == FAILURE) {
							pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "could not call class constructor");
							return 0;
						} else {
							if (!Z_ISUNDEF(stmt->fetch.cls.retval)) {
								zval_ptr_dtor(&stmt->fetch.cls.retval);
								ZVAL_UNDEF(&stmt->fetch.cls.retval);
							}
						}
					}
				}
				break;

			case PDO_FETCH_INTO:
				if (Z_ISUNDEF(stmt->fetch.into)) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "No fetch-into object specified.");
					return 0;
					break;
				}

				ZVAL_COPY(return_value, &stmt->fetch.into);

				if (Z_OBJ_P(return_value)->ce == ZEND_STANDARD_CLASS_DEF_PTR) {
					how = PDO_FETCH_OBJ;
				}
				break;

			case PDO_FETCH_FUNC:
				if (Z_ISUNDEF(stmt->fetch.func.function)) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "No fetch function specified");
					return 0;
				}
				if (!stmt->fetch.func.fci.size) {
					if (!do_fetch_func_prepare(stmt))
					{
						return 0;
					}
				}
				break;


			default:
				/* shouldn't happen */
				return 0;
		}

		if (return_all && how != PDO_FETCH_KEY_PAIR) {
			if (flags == PDO_FETCH_GROUP && how == PDO_FETCH_COLUMN && stmt->fetch.column > 0) {
				fetch_value(stmt, &grp_val, colno, NULL);
			} else {
				fetch_value(stmt, &grp_val, i, NULL);
			}
			convert_to_string(&grp_val);
			if (how == PDO_FETCH_COLUMN) {
				i = stmt->column_count; /* no more data to fetch */
			} else {
				i++;
			}
		}

		for (idx = 0; i < stmt->column_count; i++, idx++) {
			zval val;
			fetch_value(stmt, &val, i, NULL);

			switch (how) {
				case PDO_FETCH_ASSOC:
					zend_symtable_update(Z_ARRVAL_P(return_value), stmt->columns[i].name, &val);
					break;

				case PDO_FETCH_KEY_PAIR:
					{
						zval tmp;
						fetch_value(stmt, &tmp, ++i, NULL);

						if (Z_TYPE(val) == IS_LONG) {
							zend_hash_index_update((return_all ? Z_ARRVAL_P(return_all) : Z_ARRVAL_P(return_value)), Z_LVAL(val), &tmp);
						} else {
							convert_to_string(&val);
							zend_symtable_update((return_all ? Z_ARRVAL_P(return_all) : Z_ARRVAL_P(return_value)), Z_STR(val), &tmp);
						}
						zval_ptr_dtor(&val);
						return 1;
					}
					break;

				case PDO_FETCH_USE_DEFAULT:
				case PDO_FETCH_BOTH:
					zend_symtable_update(Z_ARRVAL_P(return_value), stmt->columns[i].name, &val);
					if (zend_hash_index_add(Z_ARRVAL_P(return_value), i, &val) != NULL) {
						Z_TRY_ADDREF(val);
					}
					break;

				case PDO_FETCH_NAMED:
					/* already have an item with this name? */
					{
						zval *curr_val;
						if ((curr_val = zend_hash_find(Z_ARRVAL_P(return_value), stmt->columns[i].name))) {
							zval arr;
							if (Z_TYPE_P(curr_val) != IS_ARRAY) {
								/* a little bit of black magic here:
								 * we're creating a new array and swapping it for the
								 * zval that's already stored in the hash under the name
								 * we want.  We then add that zval to the array.
								 * This is effectively the same thing as:
								 * if (!is_array($hash[$name])) {
								 *   $hash[$name] = array($hash[$name]);
								 * }
								 * */
								zval cur;

								array_init(&arr);

								ZVAL_COPY_VALUE(&cur, curr_val);
								ZVAL_COPY_VALUE(curr_val, &arr);

								zend_hash_next_index_insert_new(Z_ARRVAL(arr), &cur);
							} else {
								ZVAL_COPY_VALUE(&arr, curr_val);
							}
							zend_hash_next_index_insert_new(Z_ARRVAL(arr), &val);
						} else {
							zend_hash_update(Z_ARRVAL_P(return_value), stmt->columns[i].name, &val);
						}
					}
					break;

				case PDO_FETCH_NUM:
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &val);
					break;

				case PDO_FETCH_OBJ:
				case PDO_FETCH_INTO:
					zend_update_property_ex(NULL, Z_OBJ_P(return_value),
						stmt->columns[i].name,
						&val);
					zval_ptr_dtor(&val);
					break;

				case PDO_FETCH_CLASS:
					if ((flags & PDO_FETCH_SERIALIZE) == 0 || idx) {
						zend_update_property_ex(ce, Z_OBJ_P(return_value),
							stmt->columns[i].name,
							&val);
						zval_ptr_dtor(&val);
					} else {
						if (!ce->unserialize) {
							zval_ptr_dtor(&val);
							pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "cannot unserialize class");
							return 0;
						} else if (ce->unserialize(return_value, ce, (unsigned char *)(Z_TYPE(val) == IS_STRING ? Z_STRVAL(val) : ""), Z_TYPE(val) == IS_STRING ? Z_STRLEN(val) : 0, NULL) == FAILURE) {
							zval_ptr_dtor(&val);
							pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "cannot unserialize class");
							zval_ptr_dtor(return_value);
							ZVAL_NULL(return_value);
							return 0;
						} else {
							zval_ptr_dtor(&val);
						}
					}
					break;

				case PDO_FETCH_FUNC:
					ZVAL_COPY_VALUE(&stmt->fetch.func.values[idx], &val);
					ZVAL_COPY_VALUE(&stmt->fetch.cls.fci.params[idx], &stmt->fetch.func.values[idx]);
					break;

				default:
					zval_ptr_dtor(&val);
					pdo_raise_impl_error(stmt->dbh, stmt, "22003", "mode is out of range");
					return 0;
					break;
			}
		}

		switch (how) {
			case PDO_FETCH_CLASS:
				if (ce->constructor && !(flags & (PDO_FETCH_PROPS_LATE | PDO_FETCH_SERIALIZE))) {
					stmt->fetch.cls.fci.object = Z_OBJ_P(return_value);
					stmt->fetch.cls.fcc.object = Z_OBJ_P(return_value);
					if (zend_call_function(&stmt->fetch.cls.fci, &stmt->fetch.cls.fcc) == FAILURE) {
						pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "could not call class constructor");
						return 0;
					} else {
						if (!Z_ISUNDEF(stmt->fetch.cls.retval)) {
							zval_ptr_dtor(&stmt->fetch.cls.retval);
						}
					}
				}
				if (flags & PDO_FETCH_CLASSTYPE) {
					do_fetch_opt_finish(stmt, 0);
					stmt->fetch.cls.ce = old_ce;
					ZVAL_COPY_VALUE(&stmt->fetch.cls.ctor_args, &old_ctor_args);
					stmt->fetch.cls.fci.param_count = old_arg_count;
				}
				break;

			case PDO_FETCH_FUNC:
				stmt->fetch.func.fci.param_count = idx;
				stmt->fetch.func.fci.retval = &retval;
				if (zend_call_function(&stmt->fetch.func.fci, &stmt->fetch.func.fcc) == FAILURE) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "could not call user-supplied function");
					return 0;
				} else {
					if (return_all) {
						zval_ptr_dtor(return_value); /* we don't need that */
						ZVAL_COPY_VALUE(return_value, &retval);
					} else if (!Z_ISUNDEF(retval)) {
						ZVAL_COPY_VALUE(return_value, &retval);
					}
				}
				while (idx--) {
					zval_ptr_dtor(&stmt->fetch.func.values[idx]);
				}
				break;

			default:
				break;
		}

		if (return_all) {
			if ((flags & PDO_FETCH_UNIQUE) == PDO_FETCH_UNIQUE) {
				zend_symtable_update(Z_ARRVAL_P(return_all), Z_STR(grp_val), return_value);
			} else {
				zval grp;
				if ((pgrp = zend_symtable_find(Z_ARRVAL_P(return_all), Z_STR(grp_val))) == NULL) {
					array_init(&grp);
					zend_symtable_update(Z_ARRVAL_P(return_all), Z_STR(grp_val), &grp);
				} else {
					ZVAL_COPY_VALUE(&grp, pgrp);
				}
				zend_hash_next_index_insert(Z_ARRVAL(grp), return_value);
			}
			zval_ptr_dtor_str(&grp_val);
		}

	}

	return 1;
}
/* }}} */

static int pdo_stmt_verify_mode(pdo_stmt_t *stmt, zend_long mode, int fetch_all) /* {{{ */
{
	int flags = mode & PDO_FETCH_FLAGS;

	mode = mode & ~PDO_FETCH_FLAGS;

	if (mode < 0 || mode > PDO_FETCH__MAX) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "invalid fetch mode");
		return 0;
	}

	if (mode == PDO_FETCH_USE_DEFAULT) {
		flags = stmt->default_fetch_type & PDO_FETCH_FLAGS;
		mode = stmt->default_fetch_type & ~PDO_FETCH_FLAGS;
	}

	switch(mode) {
		case PDO_FETCH_FUNC:
			if (!fetch_all) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO::FETCH_FUNC is only allowed in PDOStatement::fetchAll()");
				return 0;
			}
			return 1;

		case PDO_FETCH_LAZY:
			if (fetch_all) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO::FETCH_LAZY can't be used with PDOStatement::fetchAll()");
				return 0;
			}
			/* fall through */
		default:
			if ((flags & PDO_FETCH_SERIALIZE) == PDO_FETCH_SERIALIZE) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO::FETCH_SERIALIZE can only be used together with PDO::FETCH_CLASS");
				return 0;
			}
			if ((flags & PDO_FETCH_CLASSTYPE) == PDO_FETCH_CLASSTYPE) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO::FETCH_CLASSTYPE can only be used together with PDO::FETCH_CLASS");
				return 0;
			}
			if (mode >= PDO_FETCH__MAX) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "invalid fetch mode");
				return 0;
			}
			/* no break; */

		case PDO_FETCH_CLASS:
			return 1;
	}
}
/* }}} */

/* {{{ Fetches the next row and returns it, or false if there are no more rows */
PHP_METHOD(PDOStatement, fetch)
{
	zend_long how = PDO_FETCH_USE_DEFAULT;
	zend_long ori = PDO_FETCH_ORI_NEXT;
	zend_long off = 0;

	ZEND_PARSE_PARAMETERS_START(0, 3)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(how)
		Z_PARAM_LONG(ori)
		Z_PARAM_LONG(off)
	ZEND_PARSE_PARAMETERS_END();

    PHP_STMT_GET_OBJ;
	PDO_STMT_CLEAR_ERR();

	if (!pdo_stmt_verify_mode(stmt, how, 0)) {
		RETURN_FALSE;
	}

	if (!do_fetch(stmt, TRUE, return_value, how, ori, off, 0)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Fetches the next row and returns it as an object. */
PHP_METHOD(PDOStatement, fetchObject)
{
	zend_long how = PDO_FETCH_CLASS;
	zend_long ori = PDO_FETCH_ORI_NEXT;
	zend_long off = 0;
	zend_class_entry *ce = NULL;
	zend_class_entry *old_ce;
	zval old_ctor_args, *ctor_args = NULL;
	int old_arg_count;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_CLASS_OR_NULL(ce)
		Z_PARAM_ARRAY(ctor_args)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	PDO_STMT_CLEAR_ERR();

	if (!pdo_stmt_verify_mode(stmt, how, 0)) {
		RETURN_FALSE;
	}

	old_ce = stmt->fetch.cls.ce;
	ZVAL_COPY_VALUE(&old_ctor_args, &stmt->fetch.cls.ctor_args);
	old_arg_count = stmt->fetch.cls.fci.param_count;

	do_fetch_opt_finish(stmt, 0);

	if (ctor_args) {
		if (Z_TYPE_P(ctor_args) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(ctor_args))) {
			ZVAL_ARR(&stmt->fetch.cls.ctor_args, zend_array_dup(Z_ARRVAL_P(ctor_args)));
		} else {
			ZVAL_UNDEF(&stmt->fetch.cls.ctor_args);
		}
	}
	if (ce) {
		stmt->fetch.cls.ce = ce;
	} else {
		stmt->fetch.cls.ce = zend_standard_class_def;
	}

	if (!do_fetch(stmt, TRUE, return_value, how, ori, off, 0)) {
		PDO_HANDLE_STMT_ERR();
		RETVAL_FALSE;
	}
	do_fetch_opt_finish(stmt, 1);

	stmt->fetch.cls.ce = old_ce;
	ZVAL_COPY_VALUE(&stmt->fetch.cls.ctor_args, &old_ctor_args);
	stmt->fetch.cls.fci.param_count = old_arg_count;
}
/* }}} */

/* {{{ Returns a data of the specified column in the result set. */
PHP_METHOD(PDOStatement, fetchColumn)
{
	zend_long col_n = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(col_n)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	PDO_STMT_CLEAR_ERR();

	if (!do_fetch_common(stmt, PDO_FETCH_ORI_NEXT, 0, TRUE)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}

	fetch_value(stmt, return_value, col_n, NULL);
}
/* }}} */

/* {{{ Returns an array of all of the results. */
PHP_METHOD(PDOStatement, fetchAll)
{
	zend_long how = PDO_FETCH_USE_DEFAULT;
	zval data, *return_all;
	zval *arg2 = NULL;
	zend_class_entry *old_ce;
	zval old_ctor_args, *ctor_args = NULL;
	int error = 0, flags, old_arg_count;

	ZEND_PARSE_PARAMETERS_START(0, 3)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(how)
		Z_PARAM_ZVAL(arg2)
		Z_PARAM_ZVAL(ctor_args)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	if (!pdo_stmt_verify_mode(stmt, how, 1)) {
		RETURN_FALSE;
	}

	old_ce = stmt->fetch.cls.ce;
	ZVAL_COPY_VALUE(&old_ctor_args, &stmt->fetch.cls.ctor_args);
	old_arg_count = stmt->fetch.cls.fci.param_count;

	do_fetch_opt_finish(stmt, 0);

	switch(how & ~PDO_FETCH_FLAGS) {
	case PDO_FETCH_CLASS:
		switch(ZEND_NUM_ARGS()) {
		case 0:
		case 1:
			stmt->fetch.cls.ce = zend_standard_class_def;
			break;
		case 3:
			if (Z_TYPE_P(ctor_args) != IS_NULL && Z_TYPE_P(ctor_args) != IS_ARRAY) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "ctor_args must be either NULL or an array");
				error = 1;
				break;
			}
			if (Z_TYPE_P(ctor_args) != IS_ARRAY || !zend_hash_num_elements(Z_ARRVAL_P(ctor_args))) {
				ctor_args = NULL;
			}
			/* no break */
		case 2:
			if (ctor_args) {
				ZVAL_COPY_VALUE(&stmt->fetch.cls.ctor_args, ctor_args); /* we're not going to free these */
			} else {
				ZVAL_UNDEF(&stmt->fetch.cls.ctor_args);
			}
			if (Z_TYPE_P(arg2) != IS_STRING) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "Invalid class name (should be a string)");
				error = 1;
				break;
			} else {
				stmt->fetch.cls.ce = zend_fetch_class(Z_STR_P(arg2), ZEND_FETCH_CLASS_AUTO);
				if (!stmt->fetch.cls.ce) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "could not find user-specified class");
					error = 1;
					break;
				}
			}
		}
		if (!error) {
			do_fetch_class_prepare(stmt);
		}
		break;

	case PDO_FETCH_FUNC:
		switch (ZEND_NUM_ARGS()) {
			case 0:
			case 1:
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "no fetch function specified");
				error = 1;
				break;
			case 3:
			case 2:
				ZVAL_COPY_VALUE(&stmt->fetch.func.function, arg2);
				if (do_fetch_func_prepare(stmt) == 0) {
					error = 1;
				}
				break;
		}
		break;

	case PDO_FETCH_COLUMN:
		switch(ZEND_NUM_ARGS()) {
		case 0:
		case 1:
			stmt->fetch.column = how & PDO_FETCH_GROUP ? -1 : 0;
			break;
		case 2:
			convert_to_long(arg2);
			stmt->fetch.column = Z_LVAL_P(arg2);
			break;
		case 3:
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "Third parameter not allowed for PDO::FETCH_COLUMN");
			error = 1;
		}
		break;

	default:
		if (ZEND_NUM_ARGS() > 1) {
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "Extraneous additional parameters");
			error = 1;
		}
	}

	flags = how & PDO_FETCH_FLAGS;

	if ((how & ~PDO_FETCH_FLAGS) == PDO_FETCH_USE_DEFAULT) {
		flags |= stmt->default_fetch_type & PDO_FETCH_FLAGS;
		how |= stmt->default_fetch_type & ~PDO_FETCH_FLAGS;
	}

	if (!error)	{
		PDO_STMT_CLEAR_ERR();
		if ((how & PDO_FETCH_GROUP) || how == PDO_FETCH_KEY_PAIR ||
			(how == PDO_FETCH_USE_DEFAULT && stmt->default_fetch_type == PDO_FETCH_KEY_PAIR)
		) {
			array_init(return_value);
			return_all = return_value;
		} else {
			return_all = 0;
		}
		if (!do_fetch(stmt, 1, &data, how | flags, PDO_FETCH_ORI_NEXT, 0, return_all)) {
			error = 2;
		}
	}
	if (!error) {
		if ((how & PDO_FETCH_GROUP)) {
			while (do_fetch(stmt, 1, &data, how | flags, PDO_FETCH_ORI_NEXT, 0, return_all));
		} else if (how == PDO_FETCH_KEY_PAIR || (how == PDO_FETCH_USE_DEFAULT && stmt->default_fetch_type == PDO_FETCH_KEY_PAIR)) {
			while (do_fetch(stmt, 1, &data, how | flags, PDO_FETCH_ORI_NEXT, 0, return_all));
		} else {
			array_init(return_value);
			do {
				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &data);
			} while (do_fetch(stmt, 1, &data, how | flags, PDO_FETCH_ORI_NEXT, 0, 0));
		}
	}

	do_fetch_opt_finish(stmt, 0);

	stmt->fetch.cls.ce = old_ce;
	ZVAL_COPY_VALUE(&stmt->fetch.cls.ctor_args, &old_ctor_args);
	stmt->fetch.cls.fci.param_count = old_arg_count;

	if (error) {
		PDO_HANDLE_STMT_ERR();
		if (error != 2) {
			RETURN_FALSE;
		} else { /* on no results, return an empty array */
			if (Z_TYPE_P(return_value) != IS_ARRAY) {
				array_init(return_value);
			}
			return;
		}
	}
}
/* }}} */

static void register_bound_param(INTERNAL_FUNCTION_PARAMETERS, int is_param) /* {{{ */
{
	struct pdo_bound_param_data param;
	zend_long param_type = PDO_PARAM_STR;
	zval *parameter, *driver_params = NULL;

	memset(&param, 0, sizeof(param));

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR_OR_LONG(param.name, param.paramno)
		Z_PARAM_ZVAL(parameter)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(param_type)
		Z_PARAM_LONG(param.max_value_len)
		Z_PARAM_ZVAL_OR_NULL(driver_params)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;

	param.param_type = (int) param_type;

	if (param.name) {
		param.paramno = -1;
	} else if (param.paramno > 0) {
		--param.paramno; /* make it zero-based internally */
	} else {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "Columns/Parameters are 1-based");
		RETURN_FALSE;
	}

	if (driver_params) {
		ZVAL_COPY(&param.driver_params, driver_params);
	}

	ZVAL_COPY(&param.parameter, parameter);
	if (!really_register_bound_param(&param, stmt, is_param)) {
		if (!Z_ISUNDEF(param.parameter)) {
			zval_ptr_dtor(&(param.parameter));
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
} /* }}} */

/* {{{ bind an input parameter to the value of a PHP variable.  $paramno is the 1-based position of the placeholder in the SQL statement (but can be the parameter name for drivers that support named placeholders).  It should be called prior to execute(). */
PHP_METHOD(PDOStatement, bindValue)
{
	struct pdo_bound_param_data param;
	zend_long param_type = PDO_PARAM_STR;
	zval *parameter;

	memset(&param, 0, sizeof(param));

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR_OR_LONG(param.name, param.paramno)
		Z_PARAM_ZVAL(parameter)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(param_type)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	param.param_type = (int) param_type;

	if (param.name) {
		param.paramno = -1;
	} else if (param.paramno > 0) {
		--param.paramno; /* make it zero-based internally */
	} else {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "Columns/Parameters are 1-based");
		RETURN_FALSE;
	}

	ZVAL_COPY(&param.parameter, parameter);
	if (!really_register_bound_param(&param, stmt, TRUE)) {
		if (!Z_ISUNDEF(param.parameter)) {
			zval_ptr_dtor(&(param.parameter));
			ZVAL_UNDEF(&param.parameter);
		}
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ bind a parameter to a PHP variable.  $paramno is the 1-based position of the placeholder in the SQL statement (but can be the parameter name for drivers that support named placeholders).  This isn't supported by all drivers.  It should be called prior to execute(). */
PHP_METHOD(PDOStatement, bindParam)
{
	register_bound_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ bind a column to a PHP variable.  On each row fetch $param will contain the value of the corresponding column.  $column is the 1-based offset of the column, or the column name.  For portability, don't call this before execute(). */
PHP_METHOD(PDOStatement, bindColumn)
{
	register_bound_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Returns the number of rows in a result set, or the number of rows affected by the last execute().  It is not always meaningful. */
PHP_METHOD(PDOStatement, rowCount)
{
	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	RETURN_LONG(stmt->row_count);
}
/* }}} */

/* {{{ Fetch the error code associated with the last operation on the statement handle */
PHP_METHOD(PDOStatement, errorCode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	if (stmt->error_code[0] == '\0') {
		RETURN_NULL();
	}

	RETURN_STRING(stmt->error_code);
}
/* }}} */

/* {{{ Fetch extended error information associated with the last operation on the statement handle */
PHP_METHOD(PDOStatement, errorInfo)
{
	int error_count;
	int error_count_diff     = 0;
	int error_expected_count = 3;

	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	array_init(return_value);
	add_next_index_string(return_value, stmt->error_code);

	if (stmt->dbh->methods->fetch_err) {
		stmt->dbh->methods->fetch_err(stmt->dbh, stmt, return_value);
	}

	error_count = zend_hash_num_elements(Z_ARRVAL_P(return_value));

	if (error_expected_count > error_count) {
		int current_index;

		error_count_diff = error_expected_count - error_count;
		for (current_index = 0; current_index < error_count_diff; current_index++) {
			add_next_index_null(return_value);
		}
	}
}
/* }}} */

/* {{{ Set an attribute */
PHP_METHOD(PDOStatement, setAttribute)
{
	zend_long attr;
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(attr)
		Z_PARAM_ZVAL_OR_NULL(value)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	if (!stmt->methods->set_attribute) {
		goto fail;
	}

	PDO_STMT_CLEAR_ERR();
	if (stmt->methods->set_attribute(stmt, attr, value)) {
		RETURN_TRUE;
	}

fail:
	if (!stmt->methods->set_attribute) {
		pdo_raise_impl_error(stmt->dbh, stmt, "IM001", "This driver doesn't support setting attributes");
	} else {
		PDO_HANDLE_STMT_ERR();
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Get an attribute */

static int generic_stmt_attr_get(pdo_stmt_t *stmt, zval *return_value, zend_long attr)
{
	switch (attr) {
		case PDO_ATTR_EMULATE_PREPARES:
			RETVAL_BOOL(stmt->supports_placeholders == PDO_PLACEHOLDER_NONE);
			return 1;
	}
	return 0;
}

PHP_METHOD(PDOStatement, getAttribute)
{
	zend_long attr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(attr)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	if (!stmt->methods->get_attribute) {
		if (!generic_stmt_attr_get(stmt, return_value, attr)) {
			pdo_raise_impl_error(stmt->dbh, stmt, "IM001",
				"This driver doesn't support getting attributes");
			RETURN_FALSE;
		}
		return;
	}

	PDO_STMT_CLEAR_ERR();
	switch (stmt->methods->get_attribute(stmt, attr, return_value)) {
		case -1:
			PDO_HANDLE_STMT_ERR();
			RETURN_FALSE;

		case 0:
			if (!generic_stmt_attr_get(stmt, return_value, attr)) {
				/* XXX: should do something better here */
				pdo_raise_impl_error(stmt->dbh, stmt, "IM001",
					"driver doesn't support getting that attribute");
				RETURN_FALSE;
			}
			return;

		default:
			return;
	}
}
/* }}} */

/* {{{ Returns the number of columns in the result set */
PHP_METHOD(PDOStatement, columnCount)
{
	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	RETURN_LONG(stmt->column_count);
}
/* }}} */

/* {{{ Returns meta data for a numbered column */
PHP_METHOD(PDOStatement, getColumnMeta)
{
	zend_long colno;
	struct pdo_column_data *col;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(colno)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	if(colno < 0) {
		pdo_raise_impl_error(stmt->dbh, stmt, "42P10", "column number must be non-negative");
		RETURN_FALSE;
	}

	if (!stmt->methods->get_column_meta) {
		pdo_raise_impl_error(stmt->dbh, stmt, "IM001", "driver doesn't support meta data");
		RETURN_FALSE;
	}

	PDO_STMT_CLEAR_ERR();
	if (FAILURE == stmt->methods->get_column_meta(stmt, colno, return_value)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}

	/* add stock items */
	col = &stmt->columns[colno];
	add_assoc_str(return_value, "name", zend_string_copy(col->name));
	add_assoc_long(return_value, "len", col->maxlen); /* FIXME: unsigned ? */
	add_assoc_long(return_value, "precision", col->precision);
	if (col->param_type != PDO_PARAM_ZVAL) {
		/* if param_type is PDO_PARAM_ZVAL the driver has to provide correct data */
		add_assoc_long(return_value, "pdo_type", col->param_type);
	}
}
/* }}} */

/* {{{ Changes the default fetch mode for subsequent fetches (params have different meaning for different fetch modes) */

int pdo_stmt_setup_fetch_mode(pdo_stmt_t *stmt, zend_long mode, zval *args, uint32_t num_args)
{
	int flags = 0;
	zend_class_entry *cep;
	int retval;

	do_fetch_opt_finish(stmt, 1);

	switch (stmt->default_fetch_type) {
		case PDO_FETCH_INTO:
			if (!Z_ISUNDEF(stmt->fetch.into)) {
				zval_ptr_dtor(&stmt->fetch.into);
				ZVAL_UNDEF(&stmt->fetch.into);
			}
			break;
		default:
			;
	}

	stmt->default_fetch_type = PDO_FETCH_BOTH;

	flags = mode & PDO_FETCH_FLAGS;
	retval = pdo_stmt_verify_mode(stmt, mode, 0);

	if (FAILURE == retval) {
		PDO_STMT_CLEAR_ERR();
		return FAILURE;
	}

	retval = FAILURE;
	switch (mode & ~PDO_FETCH_FLAGS) {
		case PDO_FETCH_USE_DEFAULT:
		case PDO_FETCH_LAZY:
		case PDO_FETCH_ASSOC:
		case PDO_FETCH_NUM:
		case PDO_FETCH_BOTH:
		case PDO_FETCH_OBJ:
		case PDO_FETCH_BOUND:
		case PDO_FETCH_NAMED:
		case PDO_FETCH_KEY_PAIR:
			if (num_args != 0) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "fetch mode doesn't allow any extra arguments");
			} else {
				retval = SUCCESS;
			}
			break;

		case PDO_FETCH_COLUMN:
			if (num_args != 1) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "fetch mode requires the colno argument");
			} else	if (Z_TYPE(args[0]) != IS_LONG) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "colno must be an integer");
			} else {
				stmt->fetch.column = Z_LVAL(args[0]);
				retval = SUCCESS;
			}
			break;

		case PDO_FETCH_CLASS:
			/* Gets its class name from 1st column */
			if ((flags & PDO_FETCH_CLASSTYPE) == PDO_FETCH_CLASSTYPE) {
				if (num_args != 0) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "fetch mode doesn't allow any extra arguments");
				} else {
					stmt->fetch.cls.ce = NULL;
					retval = SUCCESS;
				}
			} else {
				if (num_args < 1) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "fetch mode requires the classname argument");
				} else if (num_args > 2) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "too many arguments");
				} else if (Z_TYPE(args[0]) != IS_STRING) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "classname must be a string");
				} else {
					cep = zend_lookup_class(Z_STR(args[0]));
					if (cep) {
						retval = SUCCESS;
						stmt->fetch.cls.ce = cep;
					}
				}
			}

			if (SUCCESS == retval) {
				ZVAL_UNDEF(&stmt->fetch.cls.ctor_args);
#ifdef ilia_0 /* we'll only need this when we have persistent statements, if ever */
				if (stmt->dbh->is_persistent) {
					php_error_docref(NULL, E_WARNING, "PHP might crash if you don't call $stmt->setFetchMode() to reset to defaults on this persistent statement.  This will be fixed in a later release");
				}
#endif
				if (num_args == 2) {
					if (Z_TYPE(args[1]) != IS_NULL && Z_TYPE(args[1]) != IS_ARRAY) {
						pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "ctor_args must be either NULL or an array");
						retval = FAILURE;
					} else if (Z_TYPE(args[1]) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL(args[1]))) {
						ZVAL_ARR(&stmt->fetch.cls.ctor_args, zend_array_dup(Z_ARRVAL(args[1])));
					}
				}

				if (SUCCESS == retval) {
					do_fetch_class_prepare(stmt);
				}
			}

			break;

		case PDO_FETCH_INTO:
			if (num_args != 1) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "fetch mode requires the object parameter");
			} else if (Z_TYPE(args[0]) != IS_OBJECT) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "object must be an object");
			} else {
				retval = SUCCESS;
			}

			if (SUCCESS == retval) {
#ifdef ilia_0 /* we'll only need this when we have persistent statements, if ever */
				if (stmt->dbh->is_persistent) {
					php_error_docref(NULL, E_WARNING, "PHP might crash if you don't call $stmt->setFetchMode() to reset to defaults on this persistent statement.  This will be fixed in a later release");
				}
#endif
				ZVAL_COPY(&stmt->fetch.into, &args[0]);
			}

			break;

		default:
			pdo_raise_impl_error(stmt->dbh, stmt, "22003", "Invalid fetch mode specified");
	}

	if (SUCCESS == retval) {
		stmt->default_fetch_type = mode;
	}

	/*
	 * PDO error (if any) has already been raised at this point.
	 *
	 * The error_code is cleared, otherwise the caller will read the
	 * last error message from the driver.
	 *
	 */
	PDO_STMT_CLEAR_ERR();

	return retval;
}

PHP_METHOD(PDOStatement, setFetchMode)
{
	zend_long fetch_mode;
	zval *args = NULL;
	uint32_t num_args = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l*", &fetch_mode, &args, &num_args) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_STMT_GET_OBJ;
	RETVAL_BOOL(pdo_stmt_setup_fetch_mode(stmt, fetch_mode, args, num_args) == SUCCESS);
}
/* }}} */

/* {{{ Advances to the next rowset in a multi-rowset statement handle. Returns true if it succeeded, false otherwise */

static int pdo_stmt_do_next_rowset(pdo_stmt_t *stmt)
{
	/* un-describe */
	if (stmt->columns) {
		int i;
		struct pdo_column_data *cols = stmt->columns;

		for (i = 0; i < stmt->column_count; i++) {
			if (cols[i].name) {
				zend_string_release_ex(cols[i].name, 0);
			}
		}
		efree(stmt->columns);
		stmt->columns = NULL;
		stmt->column_count = 0;
	}

	if (!stmt->methods->next_rowset(stmt)) {
		/* Set the executed flag to 0 to reallocate columns on next execute */
		stmt->executed = 0;
		return 0;
	}

	pdo_stmt_describe_columns(stmt);

	return 1;
}

PHP_METHOD(PDOStatement, nextRowset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	if (!stmt->methods->next_rowset) {
		pdo_raise_impl_error(stmt->dbh, stmt, "IM001", "driver does not support multiple rowsets");
		RETURN_FALSE;
	}

	PDO_STMT_CLEAR_ERR();

	if (!pdo_stmt_do_next_rowset(stmt)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Closes the cursor, leaving the statement ready for re-execution. */
PHP_METHOD(PDOStatement, closeCursor)
{
	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	if (!stmt->methods->cursor_closer) {
		/* emulate it by fetching and discarding rows */
		do {
			while (stmt->methods->fetcher(stmt, PDO_FETCH_ORI_NEXT, 0))
				;
			if (!stmt->methods->next_rowset) {
				break;
			}

			if (!pdo_stmt_do_next_rowset(stmt)) {
				break;
			}

		} while (1);
		stmt->executed = 0;
		RETURN_TRUE;
	}

	PDO_STMT_CLEAR_ERR();

	if (!stmt->methods->cursor_closer(stmt)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}
	stmt->executed = 0;
	RETURN_TRUE;
}
/* }}} */

/* {{{ A utility for internals hackers to debug parameter internals */
PHP_METHOD(PDOStatement, debugDumpParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_stream *out = php_stream_open_wrapper("php://output", "w", 0, NULL);
	struct pdo_bound_param_data *param;

	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	if (out == NULL) {
		RETURN_FALSE;
	}

	/* break into multiple operations so query string won't be truncated at FORMAT_CONV_MAX_PRECISION */
	php_stream_printf(out, "SQL: [%zd] ", stmt->query_stringlen);
	php_stream_write(out, stmt->query_string, stmt->query_stringlen);
	php_stream_write(out, "\n", 1);

	/* show parsed SQL if emulated prepares enabled */
	/* pointers will be equal if PDO::query() was invoked */
	if (stmt->active_query_string != NULL && stmt->active_query_string != stmt->query_string) {
		/* break into multiple operations so query string won't be truncated at FORMAT_CONV_MAX_PRECISION */
		php_stream_printf(out, "Sent SQL: [%zd] ", stmt->active_query_stringlen);
		php_stream_write(out, stmt->active_query_string, stmt->active_query_stringlen);
		php_stream_write(out, "\n", 1);
	}

	php_stream_printf(out, "Params:  %d\n",
		stmt->bound_params ? zend_hash_num_elements(stmt->bound_params) : 0);

	if (stmt->bound_params) {
		zend_ulong num;
		zend_string *key = NULL;
		ZEND_HASH_FOREACH_KEY_PTR(stmt->bound_params, num, key, param) {
			if (key) {
				php_stream_printf(out, "Key: Name: [%zd] %.*s\n",
					ZSTR_LEN(key), (int) ZSTR_LEN(key), ZSTR_VAL(key));
			} else {
				php_stream_printf(out, "Key: Position #" ZEND_ULONG_FMT ":\n", num);
			}

			php_stream_printf(out,
				"paramno=" ZEND_LONG_FMT "\n"
				"name=[%zd] \"%.*s\"\n"
				"is_param=%d\n"
				"param_type=%d\n",
				param->paramno, param->name ? ZSTR_LEN(param->name) : 0, param->name ? (int) ZSTR_LEN(param->name) : 0,
				param->name ? ZSTR_VAL(param->name) : "",
				param->is_param,
				param->param_type);

		} ZEND_HASH_FOREACH_END();
	}

	php_stream_close(out);
}
/* }}} */

PHP_METHOD(PDOStatement, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

/* {{{ overloaded handlers for PDOStatement class */
static zval *dbstmt_prop_write(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	pdo_stmt_t *stmt = php_pdo_stmt_fetch_object(object);

	if (strcmp(ZSTR_VAL(name), "queryString") == 0) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "property queryString is read only");
		return value;
	} else {
		return zend_std_write_property(object, name, value, cache_slot);
	}
}

static void dbstmt_prop_delete(zend_object *object, zend_string *name, void **cache_slot)
{
	pdo_stmt_t *stmt = php_pdo_stmt_fetch_object(object);

	if (strcmp(ZSTR_VAL(name), "queryString") == 0) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "property queryString is read only");
	} else {
		zend_std_unset_property(object, name, cache_slot);
	}
}

static zend_function *dbstmt_method_get(zend_object **object_pp, zend_string *method_name, const zval *key)
{
	zend_function *fbc = NULL;
	zend_string *lc_method_name;
	zend_object *object = *object_pp;

	lc_method_name = zend_string_tolower(method_name);

	if ((fbc = zend_hash_find_ptr(&object->ce->function_table, lc_method_name)) == NULL) {
		pdo_stmt_t *stmt = php_pdo_stmt_fetch_object(object);
		/* instance not created by PDO object */
		if (!stmt->dbh) {
			goto out;
		}
		/* not a pre-defined method, nor a user-defined method; check
		 * the driver specific methods */
		if (!stmt->dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_STMT]) {
			if (!pdo_hash_methods(Z_PDO_OBJECT_P(&stmt->database_object_handle),
				PDO_DBH_DRIVER_METHOD_KIND_STMT)
				|| !stmt->dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_STMT]) {
				goto out;
			}
		}

		if ((fbc = zend_hash_find_ptr(stmt->dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_STMT], lc_method_name)) == NULL) {
			goto out;
		}
		/* got it */
	}

out:
	zend_string_release_ex(lc_method_name, 0);
	if (!fbc) {
		fbc = zend_std_get_method(object_pp, method_name, key);
	}
	return fbc;
}

static int dbstmt_compare(zval *object1, zval *object2)
{
	return ZEND_UNCOMPARABLE;
}

zend_object_handlers pdo_dbstmt_object_handlers;
zend_object_handlers pdo_row_object_handlers;

PDO_API void php_pdo_free_statement(pdo_stmt_t *stmt)
{
	if (stmt->bound_params) {
		zend_hash_destroy(stmt->bound_params);
		FREE_HASHTABLE(stmt->bound_params);
		stmt->bound_params = NULL;
	}
	if (stmt->bound_param_map) {
		zend_hash_destroy(stmt->bound_param_map);
		FREE_HASHTABLE(stmt->bound_param_map);
		stmt->bound_param_map = NULL;
	}
	if (stmt->bound_columns) {
		zend_hash_destroy(stmt->bound_columns);
		FREE_HASHTABLE(stmt->bound_columns);
		stmt->bound_columns = NULL;
	}

	if (stmt->methods && stmt->methods->dtor) {
		stmt->methods->dtor(stmt);
	}
	if (stmt->active_query_string && stmt->active_query_string != stmt->query_string) {
		efree(stmt->active_query_string);
	}
	if (stmt->query_string) {
		efree(stmt->query_string);
	}

	if (stmt->columns) {
		int i;
		struct pdo_column_data *cols = stmt->columns;

		for (i = 0; i < stmt->column_count; i++) {
			if (cols[i].name) {
				zend_string_release_ex(cols[i].name, 0);
				cols[i].name = NULL;
			}
		}
		efree(stmt->columns);
		stmt->columns = NULL;
	}

	if (!Z_ISUNDEF(stmt->fetch.into) && stmt->default_fetch_type == PDO_FETCH_INTO) {
		zval_ptr_dtor(&stmt->fetch.into);
		ZVAL_UNDEF(&stmt->fetch.into);
	}

	do_fetch_opt_finish(stmt, 1);

	if (!Z_ISUNDEF(stmt->database_object_handle)) {
		zval_ptr_dtor(&stmt->database_object_handle);
	}
	zend_object_std_dtor(&stmt->std);
}

void pdo_dbstmt_free_storage(zend_object *std)
{
	pdo_stmt_t *stmt = php_pdo_stmt_fetch_object(std);
	php_pdo_free_statement(stmt);
}

zend_object *pdo_dbstmt_new(zend_class_entry *ce)
{
	pdo_stmt_t *stmt;

	stmt = zend_object_alloc(sizeof(pdo_stmt_t), ce);
	zend_object_std_init(&stmt->std, ce);
	object_properties_init(&stmt->std, ce);

	stmt->std.handlers = &pdo_dbstmt_object_handlers;

	return &stmt->std;
}
/* }}} */

/* {{{ statement iterator */

struct php_pdo_iterator {
	zend_object_iterator iter;
	zend_ulong key;
	zval fetch_ahead;
};

static void pdo_stmt_iter_dtor(zend_object_iterator *iter)
{
	struct php_pdo_iterator *I = (struct php_pdo_iterator*)iter;

	zval_ptr_dtor(&I->iter.data);

	if (!Z_ISUNDEF(I->fetch_ahead)) {
		zval_ptr_dtor(&I->fetch_ahead);
	}
}

static int pdo_stmt_iter_valid(zend_object_iterator *iter)
{
	struct php_pdo_iterator *I = (struct php_pdo_iterator*)iter;

	return Z_ISUNDEF(I->fetch_ahead) ? FAILURE : SUCCESS;
}

static zval *pdo_stmt_iter_get_data(zend_object_iterator *iter)
{
	struct php_pdo_iterator *I = (struct php_pdo_iterator*)iter;

	/* sanity */
	if (Z_ISUNDEF(I->fetch_ahead)) {
		return NULL;
	}

	return &I->fetch_ahead;
}

static void pdo_stmt_iter_get_key(zend_object_iterator *iter, zval *key)
{
	struct php_pdo_iterator *I = (struct php_pdo_iterator*)iter;

	if (I->key == (zend_ulong)-1) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, I->key);
	}
}

static void pdo_stmt_iter_move_forwards(zend_object_iterator *iter)
{
	struct php_pdo_iterator *I = (struct php_pdo_iterator*)iter;
	pdo_stmt_t *stmt = Z_PDO_STMT_P(&I->iter.data); /* for PDO_HANDLE_STMT_ERR() */

	if (!Z_ISUNDEF(I->fetch_ahead)) {
		zval_ptr_dtor(&I->fetch_ahead);
	}

	if (!do_fetch(stmt, TRUE, &I->fetch_ahead, PDO_FETCH_USE_DEFAULT,
			PDO_FETCH_ORI_NEXT, 0, 0)) {

		PDO_HANDLE_STMT_ERR();
		I->key = (zend_ulong)-1;
		ZVAL_UNDEF(&I->fetch_ahead);

		return;
	}

	I->key++;
}

static const zend_object_iterator_funcs pdo_stmt_iter_funcs = {
	pdo_stmt_iter_dtor,
	pdo_stmt_iter_valid,
	pdo_stmt_iter_get_data,
	pdo_stmt_iter_get_key,
	pdo_stmt_iter_move_forwards,
	NULL,
	NULL,
	NULL, /* get_gc */
};

zend_object_iterator *pdo_stmt_iter_get(zend_class_entry *ce, zval *object, int by_ref)
{
	pdo_stmt_t *stmt = Z_PDO_STMT_P(object);
	struct php_pdo_iterator *I;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	I = ecalloc(1, sizeof(struct php_pdo_iterator));
	zend_iterator_init(&I->iter);
	I->iter.funcs = &pdo_stmt_iter_funcs;
	Z_ADDREF_P(object);
	ZVAL_OBJ(&I->iter.data, Z_OBJ_P(object));

	if (!do_fetch(stmt, 1, &I->fetch_ahead, PDO_FETCH_USE_DEFAULT,
			PDO_FETCH_ORI_NEXT, 0, 0)) {
		PDO_HANDLE_STMT_ERR();
		I->key = (zend_ulong)-1;
		ZVAL_UNDEF(&I->fetch_ahead);
	}

	return &I->iter;
}

/* }}} */

/* {{{ overloaded handlers for PDORow class (used by PDO_FETCH_LAZY) */

static zval *row_prop_read(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv)
{
	pdo_row_t *row = (pdo_row_t *)object;
	pdo_stmt_t *stmt = row->stmt;
	int colno = -1;
	zend_long lval;

	ZVAL_NULL(rv);
	if (stmt) {
		if (is_numeric_string(ZSTR_VAL(name), ZSTR_LEN(name), &lval, NULL, 0) == IS_LONG)	{
			if (lval >= 0 && lval < stmt->column_count) {
				fetch_value(stmt, rv, lval, NULL);
			}
		} else {
			/* TODO: replace this with a hash of available column names to column
			 * numbers */
			for (colno = 0; colno < stmt->column_count; colno++) {
				if (ZSTR_LEN(stmt->columns[colno].name) == ZSTR_LEN(name) &&
				    strncmp(ZSTR_VAL(stmt->columns[colno].name), ZSTR_VAL(name), ZSTR_LEN(name)) == 0) {
					fetch_value(stmt, rv, colno, NULL);
					return rv;
				}
			}
			if (strcmp(ZSTR_VAL(name), "queryString") == 0) {
				//zval_ptr_dtor(rv);
				return zend_std_read_property(&stmt->std, name, type, cache_slot, rv);
			}
		}
	}

	return rv;
}

static zval *row_dim_read(zend_object *object, zval *member, int type, zval *rv)
{
	pdo_row_t *row = (pdo_row_t *)object;
	pdo_stmt_t *stmt = row->stmt;
	int colno = -1;
	zend_long lval;

	ZVAL_NULL(rv);
	if (stmt) {
		if (Z_TYPE_P(member) == IS_LONG) {
			if (Z_LVAL_P(member) >= 0 && Z_LVAL_P(member) < stmt->column_count) {
				fetch_value(stmt, rv, Z_LVAL_P(member), NULL);
			}
		} else if (Z_TYPE_P(member) == IS_STRING
			   && is_numeric_string(Z_STRVAL_P(member), Z_STRLEN_P(member), &lval, NULL, 0) == IS_LONG)	{
			if (lval >= 0 && lval < stmt->column_count) {
				fetch_value(stmt, rv, lval, NULL);
			}
		} else {
			if (!try_convert_to_string(member)) {
				return &EG(uninitialized_zval);
			}

			/* TODO: replace this with a hash of available column names to column
			 * numbers */
			for (colno = 0; colno < stmt->column_count; colno++) {
				if (ZSTR_LEN(stmt->columns[colno].name) == Z_STRLEN_P(member) &&
				    strncmp(ZSTR_VAL(stmt->columns[colno].name), Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0) {
					fetch_value(stmt, rv, colno, NULL);
					return rv;
				}
			}
			if (strcmp(Z_STRVAL_P(member), "queryString") == 0) {
				//zval_ptr_dtor(rv);
				return zend_std_read_property(&stmt->std, Z_STR_P(member), type, NULL, rv);
			}
		}
	}

	return rv;
}

static zval *row_prop_write(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	php_error_docref(NULL, E_WARNING, "This PDORow is not from a writable result set");
	return value;
}

static void row_dim_write(zend_object *object, zval *member, zval *value)
{
	php_error_docref(NULL, E_WARNING, "This PDORow is not from a writable result set");
}

static int row_prop_exists(zend_object *object, zend_string *name, int check_empty, void **cache_slot)
{
	pdo_row_t *row = (pdo_row_t *)object;
	pdo_stmt_t *stmt = row->stmt;
	int colno = -1;
	zend_long lval;

	if (stmt) {
		if (is_numeric_string(ZSTR_VAL(name), ZSTR_LEN(name), &lval, NULL, 0) == IS_LONG)	{
			return lval >=0 && lval < stmt->column_count;
		}

		/* TODO: replace this with a hash of available column names to column
		 * numbers */
		for (colno = 0; colno < stmt->column_count; colno++) {
			if (ZSTR_LEN(stmt->columns[colno].name) == ZSTR_LEN(name) &&
			    strncmp(ZSTR_VAL(stmt->columns[colno].name), ZSTR_VAL(name), ZSTR_LEN(name)) == 0) {
					int res;
					zval val;

					fetch_value(stmt, &val, colno, NULL);
					res = check_empty ? i_zend_is_true(&val) : Z_TYPE(val) != IS_NULL;
					zval_ptr_dtor_nogc(&val);

					return res;
			}
		}
	}

	return 0;
}

static int row_dim_exists(zend_object *object, zval *member, int check_empty)
{
	pdo_row_t *row = (pdo_row_t *)object;
	pdo_stmt_t *stmt = row->stmt;
	int colno = -1;
	zend_long lval;

	if (stmt) {
		if (Z_TYPE_P(member) == IS_LONG) {
			return Z_LVAL_P(member) >= 0 && Z_LVAL_P(member) < stmt->column_count;
		} else if (Z_TYPE_P(member) == IS_STRING) {
			if (is_numeric_string(Z_STRVAL_P(member), Z_STRLEN_P(member), &lval, NULL, 0) == IS_LONG)	{
				return lval >=0 && lval < stmt->column_count;
			}
		} else {
			if (!try_convert_to_string(member)) {
				return 0;
			}
		}

		/* TODO: replace this with a hash of available column names to column
		 * numbers */
		for (colno = 0; colno < stmt->column_count; colno++) {
			if (ZSTR_LEN(stmt->columns[colno].name) == Z_STRLEN_P(member) &&
			    strncmp(ZSTR_VAL(stmt->columns[colno].name), Z_STRVAL_P(member), Z_STRLEN_P(member)) == 0) {
					int res;
					zval val;

					fetch_value(stmt, &val, colno, NULL);
					res = check_empty ? i_zend_is_true(&val) : Z_TYPE(val) != IS_NULL;
					zval_ptr_dtor_nogc(&val);

					return res;
			}
		}
	}

	return 0;
}

static void row_prop_delete(zend_object *object, zend_string *offset, void **cache_slot)
{
	php_error_docref(NULL, E_WARNING, "Cannot delete properties from a PDORow");
}

static void row_dim_delete(zend_object *object, zval *offset)
{
	php_error_docref(NULL, E_WARNING, "Cannot delete properties from a PDORow");
}

static HashTable *row_get_properties_for(zend_object *object, zend_prop_purpose purpose)
{
	pdo_row_t *row = (pdo_row_t *)object;
	pdo_stmt_t *stmt = row->stmt;
	HashTable *props;
	int i;

	if (purpose != ZEND_PROP_PURPOSE_DEBUG || stmt == NULL) {
		return zend_std_get_properties_for(object, purpose);
	}

	if (!stmt->std.properties) {
		rebuild_object_properties(&stmt->std);
	}
	props = zend_array_dup(stmt->std.properties);
	for (i = 0; i < stmt->column_count; i++) {
		zval val;
		fetch_value(stmt, &val, i, NULL);

		zend_hash_update(props, stmt->columns[i].name, &val);
	}
	return props;
}

static zend_function *row_method_get(
	zend_object **object_pp,
	zend_string *method_name, const zval *key)
{
	zend_function *fbc;
	zend_string *lc_method_name;

	lc_method_name = zend_string_tolower(method_name);

	if ((fbc = zend_hash_find_ptr(&pdo_row_ce->function_table, lc_method_name)) == NULL) {
		zend_string_release_ex(lc_method_name, 0);
		return NULL;
	}

	zend_string_release_ex(lc_method_name, 0);

	return fbc;
}

static zend_function *row_get_ctor(zend_object *object)
{
	zend_throw_exception_ex(php_pdo_get_exception(), 0, "You may not create a PDORow manually");
	return NULL;
}

static zend_string *row_get_classname(const zend_object *object)
{
	return zend_string_init("PDORow", sizeof("PDORow") - 1, 0);
}

static int row_compare(zval *object1, zval *object2)
{
	return ZEND_UNCOMPARABLE;
}

void pdo_row_free_storage(zend_object *std)
{
	pdo_row_t *row = (pdo_row_t *)std;
	if (row->stmt) {
		ZVAL_UNDEF(&row->stmt->lazy_object_ref);
		OBJ_RELEASE(&row->stmt->std);
	}
}

zend_object *pdo_row_new(zend_class_entry *ce)
{
	pdo_row_t *row = ecalloc(1, sizeof(pdo_row_t));
	zend_object_std_init(&row->std, ce);
	row->std.handlers = &pdo_row_object_handlers;

	return &row->std;
}

void pdo_stmt_init(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PDOStatement", class_PDOStatement_methods);
	pdo_dbstmt_ce = zend_register_internal_class(&ce);
	pdo_dbstmt_ce->get_iterator = pdo_stmt_iter_get;
	pdo_dbstmt_ce->create_object = pdo_dbstmt_new;
	pdo_dbstmt_ce->serialize = zend_class_serialize_deny;
	pdo_dbstmt_ce->unserialize = zend_class_unserialize_deny;
	zend_class_implements(pdo_dbstmt_ce, 1, zend_ce_aggregate);
	zend_declare_property_null(pdo_dbstmt_ce, "queryString", sizeof("queryString")-1, ZEND_ACC_PUBLIC);

	memcpy(&pdo_dbstmt_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_dbstmt_object_handlers.offset = XtOffsetOf(pdo_stmt_t, std);
	pdo_dbstmt_object_handlers.dtor_obj = zend_objects_destroy_object;
	pdo_dbstmt_object_handlers.free_obj = pdo_dbstmt_free_storage;
	pdo_dbstmt_object_handlers.write_property = dbstmt_prop_write;
	pdo_dbstmt_object_handlers.unset_property = dbstmt_prop_delete;
	pdo_dbstmt_object_handlers.get_method = dbstmt_method_get;
	pdo_dbstmt_object_handlers.compare = dbstmt_compare;
	pdo_dbstmt_object_handlers.clone_obj = NULL;

	INIT_CLASS_ENTRY(ce, "PDORow", class_PDORow_methods);
	pdo_row_ce = zend_register_internal_class(&ce);
	pdo_row_ce->ce_flags |= ZEND_ACC_FINAL; /* when removing this a lot of handlers need to be redone */
	pdo_row_ce->create_object = pdo_row_new;
	pdo_row_ce->serialize = zend_class_serialize_deny;
	pdo_row_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&pdo_row_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_row_object_handlers.free_obj = pdo_row_free_storage;
	pdo_row_object_handlers.clone_obj = NULL;
	pdo_row_object_handlers.get_property_ptr_ptr = NULL;
	pdo_row_object_handlers.read_property = row_prop_read;
	pdo_row_object_handlers.write_property = row_prop_write;
	pdo_row_object_handlers.has_property = row_prop_exists;
	pdo_row_object_handlers.unset_property = row_prop_delete;
	pdo_row_object_handlers.read_dimension = row_dim_read;
	pdo_row_object_handlers.write_dimension = row_dim_write;
	pdo_row_object_handlers.has_dimension = row_dim_exists;
	pdo_row_object_handlers.unset_dimension = row_dim_delete;
	pdo_row_object_handlers.get_properties_for = row_get_properties_for;
	pdo_row_object_handlers.get_method = row_method_get;
	pdo_row_object_handlers.get_constructor = row_get_ctor;
	pdo_row_object_handlers.get_class_name = row_get_classname;
	pdo_row_object_handlers.compare = row_compare;
}
