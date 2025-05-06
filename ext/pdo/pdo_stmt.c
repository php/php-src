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
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Marcus Boerger <helly@php.net>                               |
  |         Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* The PDO Statement Handle Class */

#ifdef HAVE_CONFIG_H
#include <config.h>
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

#define PHP_STMT_GET_OBJ \
	pdo_stmt_t *stmt = Z_PDO_STMT_P(ZEND_THIS); \
	if (!stmt->dbh) { \
		zend_throw_error(NULL, "%s object is uninitialized", ZSTR_VAL(Z_OBJ(EX(This))->ce->name)); \
		RETURN_THROWS(); \
	} \

static inline bool rewrite_name_to_position(pdo_stmt_t *stmt, struct pdo_bound_param_data *param) /* {{{ */
{
	if (stmt->bound_param_map) {
		/* rewriting :name to ? style.
		 * We need to fixup the parameter numbers on the parameters.
		 * If we find that a given named parameter has been used twice,
		 * we will raise an error, as we can't be sure that it is safe
		 * to bind multiple parameters onto the same zval in the underlying
		 * driver */
		zend_string *name;
		int position = 0;

		if (stmt->named_rewrite_template) {
			/* this is not an error here */
			return 1;
		}
		if (!param->name) {
			/* do the reverse; map the parameter number to the name */
			if ((name = zend_hash_index_find_ptr(stmt->bound_param_map, param->paramno)) != NULL) {
				param->name = zend_string_copy(name);
				return 1;
			}
			/* TODO Error? */
			pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "parameter was not defined");
			return 0;
		}

		ZEND_HASH_FOREACH_PTR(stmt->bound_param_map, name) {
			if (!zend_string_equals(name, param->name)) {
				position++;
				continue;
			}
			if (param->paramno >= 0) {
				/* TODO Error? */
				pdo_raise_impl_error(stmt->dbh, stmt, "IM001", "PDO refuses to handle repeating the same :named parameter for multiple positions with this driver, as it might be unsafe to do so.  Consider using a separate name for each parameter instead");
				return -1;
			}
			param->paramno = position;
			return 1;
		} ZEND_HASH_FOREACH_END();
		/* TODO Error? */
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "parameter was not defined");
		return 0;
	}
	return 1;
}
/* }}} */

/* trigger callback hook for parameters */
static bool dispatch_param_event(pdo_stmt_t *stmt, enum pdo_param_event event_type) /* {{{ */
{
	bool ret = 1, is_param = 1;
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

bool pdo_stmt_describe_columns(pdo_stmt_t *stmt) /* {{{ */
{
	int col;

	stmt->columns = ecalloc(stmt->column_count, sizeof(struct pdo_column_data));

	for (col = 0; col < stmt->column_count; col++) {
		if (!stmt->methods->describer(stmt, col)) {
			return false;
		}

		/* if we are applying case conversions on column names, do so now */
		if (stmt->dbh->native_case != stmt->dbh->desired_case && stmt->dbh->desired_case != PDO_CASE_NATURAL) {
			zend_string *orig_name = stmt->columns[col].name;
			switch (stmt->dbh->desired_case) {
				case PDO_CASE_LOWER:
					stmt->columns[col].name = zend_string_tolower(orig_name);
					zend_string_release(orig_name);
					break;
				case PDO_CASE_UPPER:
					stmt->columns[col].name = zend_string_toupper(orig_name);
					zend_string_release(orig_name);
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
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
	return true;
}
/* }}} */

static void pdo_stmt_reset_columns(pdo_stmt_t *stmt) {
	if (stmt->columns) {
		int i;
		struct pdo_column_data *cols = stmt->columns;

		for (i = 0; i < stmt->column_count; i++) {
			if (cols[i].name) {
				zend_string_release_ex(cols[i].name, 0);
			}
		}
		efree(stmt->columns);
	}
	stmt->columns = NULL;
	stmt->column_count = 0;
}

/**
 * Change the column count on the statement. If it differs from the previous one,
 * discard existing columns information.
 */
PDO_API void php_pdo_stmt_set_column_count(pdo_stmt_t *stmt, int new_count)
{
	/* Columns not yet "described". */
	if (!stmt->columns) {
		stmt->column_count = new_count;
		return;
	}

	/* The column count has not changed: No need to reload columns description.
	 * Note: Do not handle attribute name change, without column count change. */
	if (new_count == stmt->column_count) {
		return;
	}

	/* Free previous columns to force reload description. */
	pdo_stmt_reset_columns(stmt);
	stmt->column_count = new_count;
}

static void pdo_get_lazy_object(pdo_stmt_t *stmt, zval *return_value) /* {{{ */
{
	if (stmt->lazy_object_ref == NULL) {
		pdo_row_t *row = zend_object_alloc(sizeof(pdo_row_t), pdo_row_ce);
		row->stmt = stmt;
		zend_object_std_init(&row->std, pdo_row_ce);
		object_properties_init(&row->std, pdo_row_ce);
		stmt->lazy_object_ref = &row->std;
		GC_ADDREF(&stmt->std);
		GC_DELREF(&row->std);
	}
	ZVAL_OBJ_COPY(return_value, stmt->lazy_object_ref);
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

static bool really_register_bound_param(struct pdo_bound_param_data *param, pdo_stmt_t *stmt, bool is_param) /* {{{ */
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
			if (zend_string_equals(stmt->columns[i].name, param->name)) {
				param->paramno = i;
				break;
			}
		}

		/* if you prepare and then execute passing an array of params keyed by names,
		 * then this will trigger, and we don't want that */
		if (param->paramno == -1) {
			/* Should this always be an Error? */
			char *tmp;
			/* TODO Error? */
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
		if (!stmt->methods->param_hook(stmt, param, PDO_PARAM_EVT_NORMALIZE)) {
			PDO_HANDLE_STMT_ERR();
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
		if (!stmt->methods->param_hook(stmt, pparam, PDO_PARAM_EVT_ALLOC)) {
			PDO_HANDLE_STMT_ERR();
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
				/* num_index is unsignend */
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
		if (stmt->active_query_string) {
			zend_string_release(stmt->active_query_string);
			stmt->active_query_string = NULL;
		}

		ret = pdo_parse_params(stmt, stmt->query_string, &stmt->active_query_string);

		if (ret == 0) {
			/* no changes were made */
			stmt->active_query_string = zend_string_copy(stmt->query_string);
			ret = 1;
		} else if (ret == -1) {
			/* something broke */
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
			PDO_HANDLE_STMT_ERR();
			RETURN_FALSE;
		}

		RETURN_BOOL(ret);
	}
	PDO_HANDLE_STMT_ERR();
	RETURN_FALSE;
}
/* }}} */

static inline void fetch_value(pdo_stmt_t *stmt, zval *dest, int colno, enum pdo_param_type *type_override) /* {{{ */
{
	if (colno < 0) {
		zend_value_error("Column index must be greater than or equal to 0");
		ZVAL_NULL(dest);
		return;
	}

	if (colno >= stmt->column_count) {
		zend_value_error("Invalid column index");
		ZVAL_NULL(dest);
		return;
	}

	ZVAL_NULL(dest);
	stmt->methods->get_col(stmt, colno, dest, type_override);

	if (Z_TYPE_P(dest) == IS_STRING && Z_STRLEN_P(dest) == 0
			&& stmt->dbh->oracle_nulls == PDO_NULL_EMPTY_STRING) {
		zval_ptr_dtor_str(dest);
		ZVAL_NULL(dest);
	}

	/* If stringification is requested, override with PDO_PARAM_STR. */
	enum pdo_param_type pdo_param_str = PDO_PARAM_STR;
	if (stmt->dbh->stringify) {
		type_override = &pdo_param_str;
	}

	if (type_override && Z_TYPE_P(dest) != IS_NULL) {
		switch (*type_override) {
			case PDO_PARAM_INT:
				convert_to_long(dest);
				break;
			case PDO_PARAM_BOOL:
				convert_to_boolean(dest);
				break;
			case PDO_PARAM_STR:
				if (Z_TYPE_P(dest) == IS_FALSE) {
					/* Return "0" rather than "", because this is what database drivers that
					 * don't have a dedicated boolean type would return. */
					zval_ptr_dtor_nogc(dest);
					ZVAL_INTERNED_STR(dest, ZSTR_CHAR('0'));
				} else if (Z_TYPE_P(dest) == IS_RESOURCE) {
					/* Convert LOB stream to string */
					php_stream *stream;
					php_stream_from_zval_no_verify(stream, dest);
					zend_string *str = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
					zval_ptr_dtor_nogc(dest);
					if (str == NULL) {
						ZVAL_EMPTY_STRING(dest);
					} else {
						ZVAL_STR(dest, str);
					}
				} else {
					convert_to_string(dest);
				}
				break;
			case PDO_PARAM_NULL:
				convert_to_null(dest);
				break;
			case PDO_PARAM_LOB:
				if (Z_TYPE_P(dest) == IS_STRING) {
					php_stream *stream =
						php_stream_memory_open(TEMP_STREAM_READONLY, Z_STR_P(dest));
					zval_ptr_dtor_str(dest);
					php_stream_to_zval(stream, dest);
				}
				break;
			default:
				break;
		}
	}

	if (Z_TYPE_P(dest) == IS_NULL && stmt->dbh->oracle_nulls == PDO_NULL_TO_STRING) {
		ZVAL_EMPTY_STRING(dest);
	}
}
/* }}} */

static bool do_fetch_common(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori, zend_long offset) /* {{{ */
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

	if (stmt->bound_columns) {
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
				fetch_value(stmt, Z_REFVAL(param->parameter), param->paramno, &param->param_type);

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

static bool pdo_do_key_pair_fetch(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori, zend_long offset, HashTable *container)
{
	if (!do_fetch_common(stmt, ori, offset)) {
		return false;
	}
	if (stmt->column_count != 2) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "PDO::FETCH_KEY_PAIR fetch mode requires the result set to contain exactly 2 columns.");
		return false;
	}

	zval key, val;
	fetch_value(stmt, &key, 0, NULL);
	fetch_value(stmt, &val, 1, NULL);

	if (Z_TYPE(key) == IS_LONG) {
		zend_hash_index_update(container, Z_LVAL(key), &val);
	} else {
		convert_to_string(&key);
		zend_symtable_update(container, Z_STR(key), &val);
	}
	zval_ptr_dtor(&key);
	return true;
}

/* Return value MUST be an initialized object */
static bool pdo_call_fetch_object_constructor(zend_function *constructor, HashTable *ctor_args, zval *return_value)
{
	zval retval_constructor_call;
	zend_fcall_info fci = { 0 };
	fci.size = sizeof(zend_fcall_info);
	fci.object = Z_OBJ_P(return_value);
	fci.retval = &retval_constructor_call;
	fci.named_params = ctor_args;
	zend_fcall_info_cache fcc = {
		.function_handler = constructor,
		.object = Z_OBJ_P(return_value),
		.called_scope = Z_OBJCE_P(return_value),
		.calling_scope = NULL,
		.closure = NULL,
	};

	zend_call_function(&fci, &fcc);
	bool failed = Z_ISUNDEF(retval_constructor_call);
	zval_ptr_dtor(&retval_constructor_call);

	return failed;
}

/* Performs a row fetch, the value is stored into return_value according to HOW.
 * retun_value MUST be safely destroyable as it will be freed if an error occurs. */
static bool do_fetch(pdo_stmt_t *stmt, zval *return_value, enum pdo_fetch_type how, enum pdo_fetch_orientation ori, zend_long offset, zval *group_key) /* {{{ */
{
	int flags;
	zend_class_entry *ce = NULL;
	HashTable *ctor_arguments = NULL;
	int column_index_to_fetch = 0;
	zval *fetch_function_params = NULL;
	uint32_t fetch_function_param_num = 0;

	if (how == PDO_FETCH_USE_DEFAULT) {
		how = stmt->default_fetch_type;
	}
	flags = how & PDO_FETCH_FLAGS;
	how = how & ~PDO_FETCH_FLAGS;

	if (!do_fetch_common(stmt, ori, offset)) {
		return false;
	}

	if (how == PDO_FETCH_BOUND) {
		RETVAL_TRUE;
		return true;
	}

	if (how == PDO_FETCH_LAZY) {
		pdo_get_lazy_object(stmt, return_value);
		return true;
	}

	/* When fetching a column we only do one value fetch, so handle it separately */
	if (how == PDO_FETCH_COLUMN) {
		int colno = stmt->fetch.column;

		if ((flags & (PDO_FETCH_GROUP|PDO_FETCH_UNIQUE)) && stmt->fetch.column == -1) {
			colno = 1;
		}

		if (colno < 0 ) {
			zend_value_error("Column index must be greater than or equal to 0");
			return false;
		}

		if (colno >= stmt->column_count) {
			zend_value_error("Invalid column index");
			return false;
		}

		if (flags == PDO_FETCH_GROUP && stmt->fetch.column == -1) {
			fetch_value(stmt, return_value, 1, NULL);
		} else if (flags == PDO_FETCH_GROUP && colno) {
			fetch_value(stmt, return_value, 0, NULL);
		} else {
			fetch_value(stmt, return_value, colno, NULL);
		}

		if (group_key) {
			if (flags == PDO_FETCH_GROUP && stmt->fetch.column > 0) {
				fetch_value(stmt, group_key, colno, NULL);
			} else {
				fetch_value(stmt, group_key, 0, NULL);
			}
			convert_to_string(group_key);
		}
		return true;
	}

	stmt->in_fetch = true;
	switch (how) {
		case PDO_FETCH_USE_DEFAULT:
		case PDO_FETCH_ASSOC:
		case PDO_FETCH_BOTH:
		case PDO_FETCH_NUM:
		case PDO_FETCH_NAMED:
			if (!group_key) {
				array_init_size(return_value, stmt->column_count);
			} else {
				array_init(return_value);
			}
			break;

		case PDO_FETCH_OBJ:
			ce = zend_standard_class_def;
			object_init(return_value);
			break;

		case PDO_FETCH_CLASS:
			ce = stmt->fetch.cls.ce;
			if (flags & PDO_FETCH_CLASSTYPE) {
				zval ce_name_from_column;
				fetch_value(stmt, &ce_name_from_column, column_index_to_fetch++, NULL);
				/* This used to use try_convert_to_string() which would silently support integers, floats, null
				 * even if any such value could not generate a valid class name, as no class was found it would
				 * then proceed to use stdClass */
				// TODO Raise PDO implementation error when the column name is not a string
				if (Z_TYPE(ce_name_from_column) == IS_STRING) {
					ce = zend_lookup_class(Z_STR(ce_name_from_column));
				}
				/* Use default CE if present */
				if (ce == NULL) {
					ce = zend_standard_class_def;
				}
				zval_ptr_dtor(&ce_name_from_column);
			} else {
				/* This can happen if the fetch flags are set via PDO::setAttribute()
				 * $pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_CLASS);
				 * See ext/pdo/tests/bug_38253.phpt */
				if (UNEXPECTED(ce == NULL)) {
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "No fetch class specified");
					goto in_fetch_error;
				}
				ctor_arguments = stmt->fetch.cls.ctor_args;
			}
			ZEND_ASSERT(ce != NULL);
			if (flags & PDO_FETCH_SERIALIZE) {
				if (!ce->unserialize) {
					/* As this option is deprecated we do not bother to mention the class name. */
					pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "cannot unserialize class");
					goto in_fetch_error;
				}
			} else {
				if (UNEXPECTED(object_init_ex(return_value, ce) != SUCCESS)) {
					goto in_fetch_error;
				}
				if (ce->constructor && (flags & PDO_FETCH_PROPS_LATE)) {
					bool failed = pdo_call_fetch_object_constructor(ce->constructor, ctor_arguments, return_value);
					if (UNEXPECTED(failed)) {
						zval_ptr_dtor(return_value);
						goto in_fetch_error;
					}
				}
			}
			break;

		case PDO_FETCH_INTO:
			/* This can happen if the fetch flags are set via PDO::setAttribute()
			 * $pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_INTO);
			 * See ext/pdo/tests/bug_38253.phpt */
			if (stmt->fetch.into == NULL) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "No fetch-into object specified.");
				goto in_fetch_error;
			}

			ZVAL_OBJ_COPY(return_value, stmt->fetch.into);

			/* We want the behaviour of fetching into an object to be called from the global scope rather
			 * than the object scope */
			ce = NULL;
			break;

		case PDO_FETCH_FUNC:
			/* This can happen if the fetch flags are set via PDO::setAttribute()
			 * $pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_FUNC);
			 * See ext/pdo/tests/bug_38253.phpt */
			if (UNEXPECTED(!ZEND_FCC_INITIALIZED(stmt->fetch.func.fcc))) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "No fetch function specified");
				goto in_fetch_error;
			}
			/* There will be at most stmt->column_count parameters.
			 * However, if we fetch a group key we will have over allocated. */
			fetch_function_params = safe_emalloc(sizeof(zval), stmt->column_count, 0);
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	if (group_key) {
		fetch_value(stmt, group_key, column_index_to_fetch, NULL);
		convert_to_string(group_key);
		column_index_to_fetch++;
	}

	if (how == PDO_FETCH_CLASS && (flags & PDO_FETCH_SERIALIZE)) {
		zval unserialization_string;
		fetch_value(stmt, &unserialization_string, column_index_to_fetch, NULL);

		const unsigned char *str = (const unsigned char*) "";
		size_t str_len = 0;
		if (Z_TYPE(unserialization_string) == IS_STRING) {
			str = (unsigned char*) Z_STRVAL(unserialization_string);
			str_len = Z_STRLEN(unserialization_string);
		}
		zend_result unserialize_res = ce->unserialize(return_value, ce, str, str_len, NULL);
		zval_ptr_dtor(&unserialization_string);
		if (unserialize_res == FAILURE) {
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "cannot unserialize class");
			zval_ptr_dtor(return_value);
			goto in_fetch_error;
		}
		column_index_to_fetch++;
	}

	for (; column_index_to_fetch < stmt->column_count; column_index_to_fetch++) {
		zval val;
		fetch_value(stmt, &val, column_index_to_fetch, NULL);
		zend_string *column_name = stmt->columns[column_index_to_fetch].name;

		switch (how) {
			case PDO_FETCH_ASSOC:
				zend_symtable_update(Z_ARRVAL_P(return_value), column_name, &val);
				break;

			case PDO_FETCH_USE_DEFAULT:
			case PDO_FETCH_BOTH:
				zend_symtable_update(Z_ARRVAL_P(return_value), column_name, &val);
				if (zend_hash_index_add(Z_ARRVAL_P(return_value), column_index_to_fetch, &val) != NULL) {
					Z_TRY_ADDREF(val);
				}
				break;

			case PDO_FETCH_NAMED:
				/* already have an item with this name? */
				{
					zval *curr_val = zend_hash_find(Z_ARRVAL_P(return_value), column_name);
					if (curr_val != NULL) {
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
						zend_hash_update(Z_ARRVAL_P(return_value), column_name, &val);
					}
				}
				break;

			case PDO_FETCH_NUM:
				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &val);
				break;

			case PDO_FETCH_CLASS:
			case PDO_FETCH_OBJ:
			case PDO_FETCH_INTO:
				zend_update_property_ex(ce, Z_OBJ_P(return_value), column_name, &val);
				zval_ptr_dtor(&val);
				break;

			case PDO_FETCH_FUNC:
				ZVAL_COPY_VALUE(&fetch_function_params[fetch_function_param_num++], &val);
				break;
			EMPTY_SWITCH_DEFAULT_CASE();
		}
	}

	/* Run constructor for objects if not already run and not unserialized */
	if (how == PDO_FETCH_CLASS && ce->constructor && !(flags & (PDO_FETCH_PROPS_LATE | PDO_FETCH_SERIALIZE))) {
		bool failed = pdo_call_fetch_object_constructor(ce->constructor, ctor_arguments, return_value);
		if (UNEXPECTED(failed)) {
			zval_ptr_dtor(return_value);
			goto in_fetch_error;
		}
	} else if (how == PDO_FETCH_FUNC) {
		zend_call_known_fcc(&stmt->fetch.func.fcc, return_value, fetch_function_param_num, fetch_function_params, NULL);
		/* Free FCI parameters that were allocated in the previous loop */
		for (uint32_t param_num = 0; param_num < fetch_function_param_num; param_num++) {
			zval_ptr_dtor(&fetch_function_params[param_num]);
		}
		efree(fetch_function_params);
	}
	stmt->in_fetch = false;
	return true;

in_fetch_error:
	stmt->in_fetch = false;
	return false;
}
/* }}} */


// TODO Error on the following cases:
// Combining PDO_FETCH_UNIQUE and PDO_FETCH_GROUP
// Reject $pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, value); bypass
static bool pdo_verify_fetch_mode(uint32_t default_mode_and_flags, zend_long mode_and_flags, uint32_t mode_arg_num, bool fetch_all) /* {{{ */
{
	/* Mode must be a positive */
	if (mode_and_flags < 0 || mode_and_flags >= PDO_FIRST_INVALID_FLAG) {
		zend_argument_value_error(mode_arg_num, "must be a bitmask of PDO::FETCH_* constants");
		return false;
	}

	uint32_t flags = (zend_ulong)mode_and_flags & PDO_FETCH_FLAGS;
	enum pdo_fetch_type mode = (zend_ulong)mode_and_flags & ~PDO_FETCH_FLAGS;

	if (mode == PDO_FETCH_USE_DEFAULT) {
		flags = default_mode_and_flags & PDO_FETCH_FLAGS;
		mode = default_mode_and_flags & ~PDO_FETCH_FLAGS;
	}

	/* Flags can only be used in limited circumstances */
	if (flags != 0) {
		bool has_class_flags = (flags & (PDO_FETCH_CLASSTYPE|PDO_FETCH_PROPS_LATE|PDO_FETCH_SERIALIZE)) != 0;
		if (has_class_flags && mode != PDO_FETCH_CLASS) {
			zend_argument_value_error(mode_arg_num, "cannot use PDO::FETCH_CLASSTYPE, PDO::FETCH_PROPS_LATE, or PDO::FETCH_SERIALIZE fetch flags with a fetch mode other than PDO::FETCH_CLASS");
			return false;
		}
		// TODO Prevent setting those flags together or not? This would affect PDO::setFetchMode()
		//bool has_grouping_flags = flags & (PDO_FETCH_GROUP|PDO_FETCH_UNIQUE);
		//if (has_grouping_flags && !fetch_all) {
		//	zend_argument_value_error(mode_arg_num, "cannot use PDO::FETCH_GROUP, or PDO::FETCH_UNIQUE fetch flags outside of PDOStatement::fetchAll()");
		//	return false;
		//}
		if (flags & PDO_FETCH_SERIALIZE) {
			php_error_docref(NULL, E_DEPRECATED, "The PDO::FETCH_SERIALIZE mode is deprecated");
			if (UNEXPECTED(EG(exception))) {
				return false;
			}
		}
	}

	switch (mode) {
		case PDO_FETCH_FUNC:
			if (!fetch_all) {
				zend_argument_value_error(mode_arg_num, "PDO::FETCH_FUNC can only be used with PDOStatement::fetchAll()");
				return false;
			}
			return true;

		case PDO_FETCH_LAZY:
			if (fetch_all) {
				zend_argument_value_error(mode_arg_num, "PDO::FETCH_LAZY cannot be used with PDOStatement::fetchAll()");
				return false;
			}
			return true;

		case PDO_FETCH_INTO:
			if (fetch_all) {
				zend_argument_value_error(mode_arg_num, "PDO::FETCH_INTO cannot be used with PDOStatement::fetchAll()");
				return false;
			}
			return true;

		case PDO_FETCH_ASSOC:
		case PDO_FETCH_NUM:
		case PDO_FETCH_BOTH:
		case PDO_FETCH_OBJ:
		case PDO_FETCH_BOUND:
		case PDO_FETCH_COLUMN:
		case PDO_FETCH_CLASS:
		case PDO_FETCH_NAMED:
		case PDO_FETCH_KEY_PAIR:
			return true;

		default:
			zend_argument_value_error(mode_arg_num, "must be a bitmask of PDO::FETCH_* constants");
			return false;
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

	if (!pdo_verify_fetch_mode(stmt->default_fetch_type, how, 1, false)) {
		RETURN_THROWS();
	}

	int fetch_mode = how & ~PDO_FETCH_FLAGS;
	if (fetch_mode == PDO_FETCH_KEY_PAIR) {
		array_init_size(return_value, 1);
		bool success = pdo_do_key_pair_fetch(stmt, ori, off, Z_ARRVAL_P(return_value));
		if (!success) {
			zval_dtor(return_value);
			PDO_HANDLE_STMT_ERR();
			RETURN_FALSE;
		}
		return;
	}
	if (!do_fetch(stmt, return_value, how, ori, off, NULL)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Fetches the next row and returns it as an object. */
PHP_METHOD(PDOStatement, fetchObject)
{
	zend_class_entry *ce = NULL;
	zend_class_entry *old_ce;
	HashTable *old_ctor_args, *ctor_args = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_CLASS_OR_NULL(ce)
		Z_PARAM_ARRAY_HT(ctor_args)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	PDO_STMT_CLEAR_ERR();

	old_ce = stmt->fetch.cls.ce;
	old_ctor_args = stmt->fetch.cls.ctor_args;

	if (ce == NULL) {
		ce = zend_standard_class_def;
	}

	if (ctor_args && zend_hash_num_elements(ctor_args) && ce->constructor == NULL) {
		zend_argument_value_error(2, "must be empty when class provided in argument #1 ($class) does not have a constructor");
		RETURN_THROWS();
	}
	stmt->fetch.cls.ce = ce;
	stmt->fetch.cls.ctor_args = ctor_args;

	if (!do_fetch(stmt, return_value, PDO_FETCH_CLASS, PDO_FETCH_ORI_NEXT, /* offset */ 0, NULL)) {
		PDO_HANDLE_STMT_ERR();
		RETVAL_FALSE;
	}

	stmt->fetch.cls.ce = old_ce;
	stmt->fetch.cls.ctor_args = old_ctor_args;
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

	if (!do_fetch_common(stmt, PDO_FETCH_ORI_NEXT, 0)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}

	fetch_value(stmt, return_value, col_n, NULL);
}
/* }}} */

static bool pdo_get_fcc_from_zval(zend_fcall_info_cache *fcc, zval *callable) {
	if (callable == NULL) {
		/* TODO use "must be of type callable" format? */
		zend_argument_type_error(2, "must be a callable, null given");
		return false;
	}

	char *is_callable_error = NULL;
	if (!zend_is_callable_ex(callable, NULL, 0, NULL, fcc, &is_callable_error)) {
		if (is_callable_error) {
			zend_type_error("%s", is_callable_error);
			efree(is_callable_error);
		} else {
			zend_type_error("User-supplied function must be a valid callback");
		}
		return false;
	}
	return true;
}

/* {{{ Returns an array of all of the results. */
PHP_METHOD(PDOStatement, fetchAll)
{
	zend_long how = PDO_FETCH_USE_DEFAULT;
	zval *arg2 = NULL;
	zend_class_entry *old_ce;
	HashTable *old_ctor_args, *ctor_args = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 3)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(how)
		Z_PARAM_ZVAL_OR_NULL(arg2)
		Z_PARAM_ARRAY_HT_OR_NULL(ctor_args)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STMT_GET_OBJ;
	if (!pdo_verify_fetch_mode(stmt->default_fetch_type, how, 1, true)) {
		RETURN_THROWS();
	}

	int fetch_mode = how & ~PDO_FETCH_FLAGS;
	int flags = how & PDO_FETCH_FLAGS;

	old_ce = stmt->fetch.cls.ce;
	old_ctor_args = stmt->fetch.cls.ctor_args;

	/* TODO Would be good to reuse part of pdo_stmt_setup_fetch_mode() in some way */
	switch (fetch_mode) {
		case PDO_FETCH_CLASS: {
			/* Figure out correct class */
			zend_class_entry *fetch_class = NULL;
			if (arg2) {
				if (Z_TYPE_P(arg2) != IS_STRING) {
					zend_argument_type_error(2, "must be of type string, %s given", zend_zval_value_name(arg2));
					RETURN_THROWS();
				}
				fetch_class = zend_lookup_class(Z_STR_P(arg2));
				if (fetch_class == NULL) {
					zend_argument_type_error(2, "must be a valid class");
					RETURN_THROWS();
				}
			} else {
				fetch_class = zend_standard_class_def;
			}

			if (ctor_args && zend_hash_num_elements(ctor_args) > 0) {
				if (fetch_class->constructor == NULL) {
					zend_argument_value_error(3, "must be empty when class provided in argument #2 ($class) does not have a constructor");
					RETURN_THROWS();
				}
				stmt->fetch.cls.ctor_args = ctor_args;
			}
			stmt->fetch.cls.ce = fetch_class;

			break;
		}

		case PDO_FETCH_FUNC: /* Cannot be a default fetch mode */
			if (ZEND_NUM_ARGS() != 2) {
				zend_string *func = get_active_function_or_method_name();
				zend_argument_count_error("%s() expects exactly 2 argument for PDO::FETCH_FUNC, %d given",
					ZSTR_VAL(func), ZEND_NUM_ARGS());
				zend_string_release(func);
				RETURN_THROWS();
			}
			if (!pdo_get_fcc_from_zval(&stmt->fetch.func.fcc, arg2)) {
				RETURN_THROWS();
			}
			break;

		case PDO_FETCH_COLUMN:
			if (ZEND_NUM_ARGS() > 2) {
				zend_string *func = get_active_function_or_method_name();
				zend_argument_count_error("%s() expects at most 2 argument for the fetch mode provided, %d given",
					ZSTR_VAL(func), ZEND_NUM_ARGS());
				zend_string_release(func);
				RETURN_THROWS();
			}
			/* Is column index passed? */
			if (arg2) {
				// Reuse convert_to_long(arg2); ?
				if (Z_TYPE_P(arg2) != IS_LONG) {
					zend_argument_type_error(2, "must be of type int, %s given", zend_zval_value_name(arg2));
					RETURN_THROWS();
				}
				if (Z_LVAL_P(arg2) < 0) {
					zend_argument_value_error(2, "must be greater than or equal to 0");
					RETURN_THROWS();
				}
				stmt->fetch.column = Z_LVAL_P(arg2);
			} else {
				stmt->fetch.column = flags & (PDO_FETCH_GROUP|PDO_FETCH_UNIQUE) ? -1 : 0;
			}
			break;

		default:
			/* No support for PDO_FETCH_INTO which takes 2 args??? */
			if (ZEND_NUM_ARGS() > 1) {
				zend_string *func = get_active_function_or_method_name();
				zend_argument_count_error("%s() expects exactly 1 argument for the fetch mode provided, %d given",
				ZSTR_VAL(func), ZEND_NUM_ARGS());
				zend_string_release(func);
				RETURN_THROWS();
			}
	}

	if (fetch_mode == PDO_FETCH_USE_DEFAULT) {
		flags |= stmt->default_fetch_type & PDO_FETCH_FLAGS;
		fetch_mode = stmt->default_fetch_type & ~PDO_FETCH_FLAGS;
		how = fetch_mode | flags;
	}

	PDO_STMT_CLEAR_ERR();

	zval data, group_key;

	ZVAL_UNDEF(&data);
	array_init(return_value);

	if (fetch_mode == PDO_FETCH_KEY_PAIR) {
		while (pdo_do_key_pair_fetch(stmt, PDO_FETCH_ORI_NEXT, /* offset */ 0, Z_ARRVAL_P(return_value)));
		PDO_HANDLE_STMT_ERR();
		return;
	}

	// Need to handle the "broken" PDO_FETCH_GROUP|PDO_FETCH_UNIQUE fetch case
	//if (flags == PDO_FETCH_GROUP || flags == PDO_FETCH_UNIQUE) {
	if (flags & PDO_FETCH_GROUP || flags & PDO_FETCH_UNIQUE) {
		while (do_fetch(stmt, &data, how | flags, PDO_FETCH_ORI_NEXT, /* offset */ 0, &group_key)) {
			ZEND_ASSERT(Z_TYPE(group_key) == IS_STRING);
			if ((flags & PDO_FETCH_UNIQUE) == PDO_FETCH_UNIQUE) {
				zend_symtable_update(Z_ARRVAL_P(return_value), Z_STR(group_key), &data);
			} else {
				zval *group_ptr = zend_symtable_find(Z_ARRVAL_P(return_value), Z_STR(group_key));
				zval group;
				if (group_ptr == NULL) {
					group_ptr = &group;
					array_init(group_ptr);
					zend_symtable_update(Z_ARRVAL_P(return_value), Z_STR(group_key), group_ptr);
				}
				zend_hash_next_index_insert(Z_ARRVAL_P(group_ptr), &data);
			}
			zval_ptr_dtor_str(&group_key);
		}
	} else {
		while (do_fetch(stmt, &data, how, PDO_FETCH_ORI_NEXT, /* offset */ 0, NULL)) {
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &data);
		}
	}

	/* Restore defaults which were changed by PDO_FETCH_CLASS mode */
	stmt->fetch.cls.ce = old_ce;
	stmt->fetch.cls.ctor_args = old_ctor_args;

	PDO_HANDLE_STMT_ERR_EX(zval_ptr_dtor(return_value); RETVAL_EMPTY_ARRAY(););
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
		if (ZSTR_LEN(param.name) == 0) {
			zend_argument_must_not_be_empty_error(1);
			RETURN_THROWS();
		}
		param.paramno = -1;
	} else if (param.paramno > 0) {
		--param.paramno; /* make it zero-based internally */
	} else {
		zend_argument_value_error(1, "must be greater than or equal to 1");
		RETURN_THROWS();
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
		if (ZSTR_LEN(param.name) == 0) {
			zend_argument_must_not_be_empty_error(1);
			RETURN_THROWS();
		}
		param.paramno = -1;
	} else if (param.paramno > 0) {
		--param.paramno; /* make it zero-based internally */
	} else {
		zend_argument_value_error(1, "must be greater than or equal to 1");
		RETURN_THROWS();
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
	int error_count_diff = 0;
	int error_expected_count = 3;

	ZEND_PARSE_PARAMETERS_NONE();

	PHP_STMT_GET_OBJ;
	array_init(return_value);
	add_next_index_string(return_value, stmt->error_code);

	if (strncmp(stmt->error_code, PDO_ERR_NONE, sizeof(PDO_ERR_NONE))) {
		if (stmt->dbh->methods->fetch_err) {
			stmt->dbh->methods->fetch_err(stmt->dbh, stmt, return_value);
		}
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

	/* Driver hasn't registered a function for setting attributes */
	if (!stmt->methods->set_attribute) {
		pdo_raise_impl_error(stmt->dbh, stmt, "IM001", "This driver doesn't support setting attributes");
		RETURN_FALSE;
	}

	PDO_STMT_CLEAR_ERR();
	if (stmt->methods->set_attribute(stmt, attr, value)) {
		RETURN_TRUE;
	}

	/* Error while setting attribute */
	PDO_HANDLE_STMT_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ Get an attribute */

static bool generic_stmt_attr_get(pdo_stmt_t *stmt, zval *return_value, zend_long attr)
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
	if (colno < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
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
}
/* }}} */

void pdo_stmt_free_default_fetch_mode(pdo_stmt_t *stmt)
{
	enum pdo_fetch_type default_fetch_mode = stmt->default_fetch_type & ~PDO_FETCH_FLAGS;
	if (default_fetch_mode == PDO_FETCH_INTO) {
		/* This can happen if the fetch flags are set via PDO::setAttribute()
		 * $pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_INTO);
		 * See ext/pdo/tests/bug_38253.phpt */
		if (EXPECTED(stmt->fetch.into != NULL)) {
			OBJ_RELEASE(stmt->fetch.into);
		}
	} else if (default_fetch_mode == PDO_FETCH_CLASS) {
		if (stmt->fetch.cls.ctor_args != NULL) {
			zend_array_release(stmt->fetch.cls.ctor_args);
		}
	}
	memset(&stmt->fetch, 0, sizeof(stmt->fetch));
}

/* {{{ Changes the default fetch mode for subsequent fetches (params have different meaning for different fetch modes) */

bool pdo_stmt_setup_fetch_mode(pdo_stmt_t *stmt, zend_long mode, uint32_t mode_arg_num,
	zval *args, uint32_t variadic_num_args)
{
	int flags = 0;
	uint32_t arg1_arg_num = mode_arg_num + 1;
	uint32_t constructor_arg_num = mode_arg_num + 2;
	uint32_t total_num_args = mode_arg_num + variadic_num_args;

	pdo_stmt_free_default_fetch_mode(stmt);

	stmt->default_fetch_type = PDO_FETCH_BOTH;

	flags = mode & PDO_FETCH_FLAGS;

	if (!pdo_verify_fetch_mode(stmt->default_fetch_type, mode, mode_arg_num, false)) {
		return false;
	}

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
			if (variadic_num_args != 0) {
				zend_string *func = get_active_function_or_method_name();
				zend_argument_count_error("%s() expects exactly %d arguments for the fetch mode provided, %d given",
					ZSTR_VAL(func), mode_arg_num, total_num_args);
				zend_string_release(func);
				return false;
			}
			break;

		case PDO_FETCH_COLUMN:
			if (variadic_num_args != 1) {
				zend_string *func = get_active_function_or_method_name();
				zend_argument_count_error("%s() expects exactly %d arguments for the fetch mode provided, %d given",
					ZSTR_VAL(func), arg1_arg_num, total_num_args);
				zend_string_release(func);
				return false;
			}
			if (Z_TYPE(args[0]) != IS_LONG) {
				zend_argument_type_error(arg1_arg_num, "must be of type int, %s given", zend_zval_value_name(&args[0]));
				return false;
			}
			if (Z_LVAL(args[0]) < 0) {
				zend_argument_value_error(arg1_arg_num, "must be greater than or equal to 0");
				return false;
			}
			stmt->fetch.column = Z_LVAL(args[0]);
			break;

		case PDO_FETCH_CLASS: {
			/* Gets its class name from 1st column */
			if ((flags & PDO_FETCH_CLASSTYPE) == PDO_FETCH_CLASSTYPE) {
				if (variadic_num_args != 0) {
					zend_string *func = get_active_function_or_method_name();
					zend_argument_count_error("%s() expects exactly %d arguments for the fetch mode provided, %d given",
						ZSTR_VAL(func), mode_arg_num, total_num_args);
					zend_string_release(func);
					return false;
				}
			} else {
				zend_class_entry *cep;
				if (variadic_num_args == 0) {
					zend_string *func = get_active_function_or_method_name();
					zend_argument_count_error("%s() expects at least %d arguments for the fetch mode provided, %d given",
						ZSTR_VAL(func), arg1_arg_num, total_num_args);
					zend_string_release(func);
					return false;
				}
				/* constructor_arguments can be null/not passed */
				if (variadic_num_args > 2) {
					zend_string *func = get_active_function_or_method_name();
					zend_argument_count_error("%s() expects at most %d arguments for the fetch mode provided, %d given",
						ZSTR_VAL(func), constructor_arg_num, total_num_args);
					zend_string_release(func);
					return false;
				}
				if (Z_TYPE(args[0]) != IS_STRING) {
					zend_argument_type_error(arg1_arg_num, "must be of type string, %s given", zend_zval_value_name(&args[0]));
					return false;
				}
				cep = zend_lookup_class(Z_STR(args[0]));
				if (!cep) {
					zend_argument_type_error(arg1_arg_num, "must be a valid class");
					return false;
				}
				/* Verify constructor_args (args[1]) is ?array */
				/* TODO: Improve logic? */
				if (variadic_num_args == 2) {
					if (Z_TYPE(args[1]) != IS_NULL && Z_TYPE(args[1]) != IS_ARRAY) {
						zend_argument_type_error(constructor_arg_num, "must be of type ?array, %s given",
							zend_zval_value_name(&args[1]));
						return false;
					}
					if (Z_TYPE(args[1]) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL(args[1]))) {
						if (cep->constructor == NULL) {
							zend_argument_value_error(3, "must be empty when class provided in argument #2 ($class) does not have a constructor");
							return false;
						}
						GC_TRY_ADDREF(Z_ARRVAL(args[1]));
						stmt->fetch.cls.ctor_args = Z_ARRVAL(args[1]);
					}
				}
				stmt->fetch.cls.ce = cep;
			}
			break;
		}
		case PDO_FETCH_INTO:
			if (total_num_args != arg1_arg_num) {
				zend_string *func = get_active_function_or_method_name();
				zend_argument_count_error("%s() expects exactly %d arguments for the fetch mode provided, %d given",
					ZSTR_VAL(func), arg1_arg_num, total_num_args);
				zend_string_release(func);
				return false;
			}
			if (Z_TYPE(args[0]) != IS_OBJECT) {
				zend_argument_type_error(arg1_arg_num, "must be of type object, %s given", zend_zval_value_name(&args[0]));
				return false;
			}

			GC_ADDREF(Z_OBJ(args[0]));
			stmt->fetch.into = Z_OBJ(args[0]);
			break;
		default:
			zend_argument_value_error(mode_arg_num, "must be one of the PDO::FETCH_* constants");
			return false;
	}

	stmt->default_fetch_type = mode;

	return true;
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

	if (stmt->in_fetch) {
		zend_throw_error(NULL, "Cannot change default fetch mode while fetching");
		RETURN_THROWS();
	}
	if (!pdo_stmt_setup_fetch_mode(stmt, fetch_mode, 1, args, num_args)) {
		RETURN_THROWS();
	}

	// TODO Void return?
	RETURN_TRUE;
}
/* }}} */

/* {{{ Advances to the next rowset in a multi-rowset statement handle. Returns true if it succeeded, false otherwise */

static bool pdo_stmt_do_next_rowset(pdo_stmt_t *stmt)
{
	pdo_stmt_reset_columns(stmt);

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
	php_stream_printf(out, "SQL: [%zd] ", ZSTR_LEN(stmt->query_string));
	php_stream_write(out, ZSTR_VAL(stmt->query_string), ZSTR_LEN(stmt->query_string));
	php_stream_write(out, "\n", 1);

	/* show parsed SQL if emulated prepares enabled */
	/* pointers will be equal if PDO::query() was invoked */
	if (stmt->active_query_string != NULL && stmt->active_query_string != stmt->query_string) {
		/* break into multiple operations so query string won't be truncated at FORMAT_CONV_MAX_PRECISION */
		php_stream_printf(out, "Sent SQL: [%zd] ", ZSTR_LEN(stmt->active_query_string));
		php_stream_write(out, ZSTR_VAL(stmt->active_query_string), ZSTR_LEN(stmt->active_query_string));
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
	if (zend_string_equals_literal(name, "queryString")) {
		zval *query_string = OBJ_PROP_NUM(object, 0);
		if (!Z_ISUNDEF_P(query_string)) {
			zend_throw_error(NULL, "Property queryString is read only");
			return value;
		}
	}
	return zend_std_write_property(object, name, value, cache_slot);
}

static void dbstmt_prop_delete(zend_object *object, zend_string *name, void **cache_slot)
{
	if (zend_string_equals_literal(name, "queryString")) {
		zend_throw_error(NULL, "Property queryString is read only");
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
			if (!pdo_hash_methods(php_pdo_dbh_fetch_object(stmt->database_object_handle),
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

static HashTable *dbstmt_get_gc(zend_object *object, zval **gc_data, int *gc_count)
{
	pdo_stmt_t *stmt = php_pdo_stmt_fetch_object(object);
	enum pdo_fetch_type default_fetch_mode = stmt->default_fetch_type & ~PDO_FETCH_FLAGS;

	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (stmt->database_object_handle) {
		zend_get_gc_buffer_add_obj(gc_buffer, stmt->database_object_handle);
	}
	if (default_fetch_mode == PDO_FETCH_INTO) {
		if (stmt->fetch.into) {
			zend_get_gc_buffer_add_obj(gc_buffer, stmt->fetch.into);
		}
	} else if (default_fetch_mode == PDO_FETCH_CLASS && stmt->fetch.cls.ctor_args != NULL) {
		zend_get_gc_buffer_add_ht(gc_buffer, stmt->fetch.cls.ctor_args);
	}
	zend_get_gc_buffer_use(gc_buffer, gc_data, gc_count);

	/**
	 * If there are no dynamic properties and the default property is 1 (that is, there is only one property
	 * of string that does not participate in GC), there is no need to call zend_std_get_properties().
	 */
	if (object->properties == NULL && object->ce->default_properties_count <= 1) {
		return NULL;
	} else {
		return zend_std_get_properties(object);
	}
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
	if (stmt->active_query_string) {
		zend_string_release(stmt->active_query_string);
	}
	if (stmt->query_string) {
		zend_string_release(stmt->query_string);
	}

	pdo_stmt_reset_columns(stmt);
	pdo_stmt_free_default_fetch_mode(stmt);

	if (stmt->database_object_handle != NULL) {
		OBJ_RELEASE(stmt->database_object_handle);
		stmt->database_object_handle = NULL;
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
		ZVAL_UNDEF(&I->fetch_ahead);
	}
}

static zend_result pdo_stmt_iter_valid(zend_object_iterator *iter)
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
		ZVAL_UNDEF(&I->fetch_ahead);
	}

	if (!do_fetch(stmt, &I->fetch_ahead, PDO_FETCH_USE_DEFAULT,
			PDO_FETCH_ORI_NEXT, /* offset */ 0, NULL)) {
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
	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	pdo_stmt_t *stmt = Z_PDO_STMT_P(object);
	if (!stmt->dbh) {
		zend_throw_error(NULL, "%s object is uninitialized", ZSTR_VAL(ce->name));
		return NULL;
	}

	struct php_pdo_iterator *I = ecalloc(1, sizeof(struct php_pdo_iterator));
	zend_iterator_init(&I->iter);
	I->iter.funcs = &pdo_stmt_iter_funcs;
	Z_ADDREF_P(object);
	ZVAL_OBJ(&I->iter.data, Z_OBJ_P(object));

	if (!do_fetch(stmt, &I->fetch_ahead, PDO_FETCH_USE_DEFAULT,
			PDO_FETCH_ORI_NEXT, /* offset */ 0, NULL)) {
		PDO_HANDLE_STMT_ERR();
		I->key = (zend_ulong)-1;
		ZVAL_UNDEF(&I->fetch_ahead);
	}

	return &I->iter;
}

/* }}} */

/* {{{ overloaded handlers for PDORow class (used by PDO_FETCH_LAZY) */
static zval *row_read_column_name(pdo_stmt_t *stmt, zend_string *name, zval *rv)
{
	/* TODO: replace this with a hash of available column names to column numbers */
	for (int colno = 0; colno < stmt->column_count; colno++) {
		if (zend_string_equals(stmt->columns[colno].name, name)) {
			fetch_value(stmt, rv, colno, NULL);
			return rv;
		}
	}
	return NULL;
}

static zval *row_read_column_number(pdo_stmt_t *stmt, zend_long column, zval *rv)
{
	if (column >= 0 && column < stmt->column_count) {
		fetch_value(stmt, rv, column, NULL);
		return rv;
	}
	return NULL;
}

static zval *row_prop_read(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv)
{
	pdo_row_t *row = php_pdo_row_fetch_object(object);
	pdo_stmt_t *stmt = row->stmt;
	zend_long lval;
	zval *retval;
	ZEND_ASSERT(stmt);

	ZVAL_NULL(rv);
	if (zend_string_equals_literal(name, "queryString")) {
		return zend_std_read_property(&stmt->std, name, type, cache_slot, rv);
	} else if (is_numeric_str_function(name, &lval, /* dval */ NULL) == IS_LONG) {
		retval = row_read_column_number(stmt, lval, rv);
	} else {
		retval = row_read_column_name(stmt, name, rv);
	}
	if (UNEXPECTED(!retval)) {
		// TODO throw an error on master
		//if (type != BP_VAR_IS) {
		//	if (is_numeric) {
		//		zend_value_error("Invalid column index");
		//	} else {
		//		zend_throw_error(NULL, "No column named \"%s\" exists", ZSTR_VAL(name));
		//	}
		//}
		//return &EG(uninitialized_zval);
		ZVAL_NULL(rv);
		return rv;
	}
	return retval;
}

static zval *row_dim_read(zend_object *object, zval *offset, int type, zval *rv)
{
	if (UNEXPECTED(!offset)) {
		zend_throw_error(NULL, "Cannot append to PDORow offset");
		return NULL;
	}
	if (Z_TYPE_P(offset) == IS_LONG) {
		pdo_row_t *row = php_pdo_row_fetch_object(object);
		pdo_stmt_t *stmt = row->stmt;
		ZEND_ASSERT(stmt);

		ZVAL_NULL(rv);
		if (Z_LVAL_P(offset) >= 0 && Z_LVAL_P(offset) < stmt->column_count) {
			fetch_value(stmt, rv, Z_LVAL_P(offset), NULL);
		}
		return rv;
	} else {
		zend_string *member = zval_try_get_string(offset);
		if (!member) {
			return NULL;
		}
		zval *result = row_prop_read(object, member, type, NULL, rv);
		zend_string_release_ex(member, false);
		return result;
	}
}

static zval *row_prop_write(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	zend_throw_error(NULL, "Cannot write to PDORow property");
	return value;
}

static void row_dim_write(zend_object *object, zval *member, zval *value)
{
	if (!member) {
		zend_throw_error(NULL, "Cannot append to PDORow offset");
	} else {
		zend_throw_error(NULL, "Cannot write to PDORow offset");
	}
}

// todo: make row_prop_exists return bool as well
static int row_prop_exists(zend_object *object, zend_string *name, int check_empty, void **cache_slot)
{
	pdo_row_t *row = php_pdo_row_fetch_object(object);
	pdo_stmt_t *stmt = row->stmt;
	zend_long lval;
	zval tmp_val;
	zval *retval = NULL;
	ZEND_ASSERT(stmt);

	if (is_numeric_str_function(name, &lval, /* dval */ NULL) == IS_LONG) {
		retval = row_read_column_number(stmt, lval, &tmp_val);
	} else {
		retval = row_read_column_name(stmt, name, &tmp_val);
	}

	if (!retval) {
		return false;
	}
	ZEND_ASSERT(retval == &tmp_val);
	bool res = check_empty ? i_zend_is_true(retval) : Z_TYPE(tmp_val) != IS_NULL;
	zval_ptr_dtor_nogc(retval);

	return res;
}


// todo: make row_dim_exists return bool as well
static int row_dim_exists(zend_object *object, zval *offset, int check_empty)
{
	if (Z_TYPE_P(offset) == IS_LONG) {
		pdo_row_t *row = php_pdo_row_fetch_object(object);
		pdo_stmt_t *stmt = row->stmt;
		ZEND_ASSERT(stmt);
		zend_long column = Z_LVAL_P(offset);

		if (!check_empty) {
			return column >= 0 && column < stmt->column_count;
		}

		zval tmp_val;
		zval *retval = row_read_column_number(stmt, column, &tmp_val);
		if (!retval) {
			return false;
		}
		ZEND_ASSERT(retval == &tmp_val);
		bool res = i_zend_is_true(retval);
		zval_ptr_dtor_nogc(retval);
		return res;
	} else {
		zend_string *member = zval_try_get_string(offset);
		if (!member) {
			return 0;
		}
		int result = row_prop_exists(object, member, check_empty, NULL);
		zend_string_release_ex(member, false);
		return result;
	}
}

static void row_prop_delete(zend_object *object, zend_string *offset, void **cache_slot)
{
	zend_throw_error(NULL, "Cannot unset PDORow property");
}

static void row_dim_delete(zend_object *object, zval *offset)
{
	zend_throw_error(NULL, "Cannot unset PDORow offset");
}

static HashTable *row_get_properties_for(zend_object *object, zend_prop_purpose purpose)
{
	pdo_row_t *row = php_pdo_row_fetch_object(object);
	pdo_stmt_t *stmt = row->stmt;
	HashTable *props;
	int i;
	ZEND_ASSERT(stmt);

	if (purpose != ZEND_PROP_PURPOSE_DEBUG) {
		return zend_std_get_properties_for(object, purpose);
	}

	props = zend_array_dup(zend_std_get_properties_ex(&stmt->std));
	for (i = 0; i < stmt->column_count; i++) {
		if (zend_string_equals_literal(stmt->columns[i].name, "queryString")) {
			continue;
		}

		zval val;
		fetch_value(stmt, &val, i, NULL);

		zend_hash_update(props, stmt->columns[i].name, &val);
	}
	return props;
}

static zend_function *row_get_ctor(zend_object *object)
{
	zend_throw_exception_ex(php_pdo_get_exception(), 0, "You may not create a PDORow manually");
	return NULL;
}

static zval *pdo_row_get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot)
{
	ZEND_IGNORE_VALUE(object);
	ZEND_IGNORE_VALUE(name);
	ZEND_IGNORE_VALUE(type);

	if (cache_slot) {
		cache_slot[0] = cache_slot[1] = cache_slot[2] = NULL;
	}
	return NULL;
}

static void pdo_row_free_storage(zend_object *std)
{
	pdo_row_t *row = php_pdo_row_fetch_object(std);
	if (row->stmt) {
		row->stmt->lazy_object_ref = NULL;
		OBJ_RELEASE(&row->stmt->std);
	}
	zend_object_std_dtor(std);
}

static zend_object *pdo_row_new(zend_class_entry *ce)
{
	pdo_row_t *row = zend_object_alloc(sizeof(pdo_row_t), ce);
	zend_object_std_init(&row->std, ce);
	object_properties_init(&row->std, ce);

	return &row->std;
}

void pdo_stmt_init(void)
{
	pdo_dbstmt_ce = register_class_PDOStatement(zend_ce_aggregate);
	pdo_dbstmt_ce->get_iterator = pdo_stmt_iter_get;
	pdo_dbstmt_ce->create_object = pdo_dbstmt_new;
	pdo_dbstmt_ce->default_object_handlers = &pdo_dbstmt_object_handlers;

	memcpy(&pdo_dbstmt_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_dbstmt_object_handlers.offset = XtOffsetOf(pdo_stmt_t, std);
	pdo_dbstmt_object_handlers.free_obj = pdo_dbstmt_free_storage;
	pdo_dbstmt_object_handlers.write_property = dbstmt_prop_write;
	pdo_dbstmt_object_handlers.unset_property = dbstmt_prop_delete;
	pdo_dbstmt_object_handlers.get_method = dbstmt_method_get;
	pdo_dbstmt_object_handlers.compare = zend_objects_not_comparable;
	pdo_dbstmt_object_handlers.clone_obj = NULL;
	pdo_dbstmt_object_handlers.get_gc = dbstmt_get_gc;

	pdo_row_ce = register_class_PDORow();
	pdo_row_ce->create_object = pdo_row_new;
	pdo_row_ce->default_object_handlers = &pdo_row_object_handlers;

	memcpy(&pdo_row_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_row_object_handlers.offset = XtOffsetOf(pdo_row_t, std);
	pdo_row_object_handlers.free_obj = pdo_row_free_storage;
	pdo_row_object_handlers.clone_obj = NULL;
	pdo_row_object_handlers.get_property_ptr_ptr = pdo_row_get_property_ptr_ptr;
	pdo_row_object_handlers.read_property = row_prop_read;
	pdo_row_object_handlers.write_property = row_prop_write;
	pdo_row_object_handlers.has_property = row_prop_exists;
	pdo_row_object_handlers.unset_property = row_prop_delete;
	pdo_row_object_handlers.read_dimension = row_dim_read;
	pdo_row_object_handlers.write_dimension = row_dim_write;
	pdo_row_object_handlers.has_dimension = row_dim_exists;
	pdo_row_object_handlers.unset_dimension = row_dim_delete;
	pdo_row_object_handlers.get_properties_for = row_get_properties_for;
	pdo_row_object_handlers.get_constructor = row_get_ctor;
	pdo_row_object_handlers.compare = zend_objects_not_comparable;
}
