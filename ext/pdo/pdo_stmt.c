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
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Marcus Boerger <helly@php.net>                               |
  |         Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* The PDO Statement Handle Class */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pdo.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"
#include "php_pdo_sql_parser.h"
#include "zend_exceptions.h"

#if COMPILE_DL_PDO
/* {{{ content from zend_arg_defs.c:
 * since it is a .c file, it won't be installed for use by PECL extensions, so we include it here. */
ZEND_BEGIN_ARG_INFO(first_arg_force_ref, 0)
	ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO();


ZEND_BEGIN_ARG_INFO(second_arg_force_ref, 0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(third_arg_force_ref, 0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO();


ZEND_BEGIN_ARG_INFO(fourth_arg_force_ref, 0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(0)
	ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(all_args_by_ref, 1)
ZEND_END_ARG_INFO();
/* }}} */
#endif

static PHP_FUNCTION(dbstmt_constructor) /* {{{ */
{
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "You should not create a PDOStatement manually");
}
/* }}} */

/* trigger callback hook for parameters */
static int dispatch_param_event(pdo_stmt_t *stmt, enum pdo_param_event event_type TSRMLS_DC)
{
	int ret = 1, is_param = 1;
	struct pdo_bound_param_data *param;
	HashTable *ht;

	if (!stmt->methods->param_hook) {
		return 1;
	}
	
	ht = stmt->bound_params;

iterate:
	if (ht) {
		zend_hash_internal_pointer_reset(ht);
		while (SUCCESS == zend_hash_get_current_data(ht, (void**)&param)) {

			if (!stmt->methods->param_hook(stmt, param, event_type TSRMLS_CC)) {
				ret = 0;
				break;
			}
			
			zend_hash_move_forward(ht);
		}
	}
	if (ret && is_param) {
		ht = stmt->bound_columns;
		is_param = 0;
		goto iterate;
	}

	return ret;
}

static int describe_columns(pdo_stmt_t *stmt TSRMLS_DC)
{
	int col;

	stmt->columns = ecalloc(stmt->column_count, sizeof(struct pdo_column_data));

	for (col = 0; col < stmt->column_count; col++) {
		if (!stmt->methods->describer(stmt, col TSRMLS_CC)) {
			return 0;
		}

		/* XXX: if we are applying case conversions on column names, do so now */

		/* update the column index on named bound parameters */
		if (stmt->dbh->placeholders_can_be_strings && stmt->bound_params) {
			struct pdo_bound_param_data *param;

			if (SUCCESS == zend_hash_find(stmt->bound_params, stmt->columns[col].name,
						stmt->columns[col].namelen, (void**)&param)) {
				param->paramno = col;
			}
		}
		if (stmt->bound_columns) {
			struct pdo_bound_param_data *param;

			if (SUCCESS == zend_hash_find(stmt->bound_columns, stmt->columns[col].name,
						stmt->columns[col].namelen, (void**)&param)) {
				param->paramno = col;
			}
		}

	}
	return 1;
}

static void param_dtor(void *data)
{
	struct pdo_bound_param_data *param = (struct pdo_bound_param_data *)data;
	TSRMLS_FETCH();

	/* tell the driver that it is going away */
	if (param->stmt->methods->param_hook) {
		param->stmt->methods->param_hook(param->stmt, param, PDO_PARAM_EVT_FREE TSRMLS_CC);
	}
	
	zval_ptr_dtor(&(param->parameter));
	zval_ptr_dtor(&(param->driver_params));

}

static int really_register_bound_param(struct pdo_bound_param_data *param, pdo_stmt_t *stmt, int is_param TSRMLS_DC)
{
	HashTable *hash;
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

	if (param->param_type == PDO_PARAM_STR && param->max_value_len <= 0) {
		convert_to_string(param->parameter);
		/* XXX: need to provide a way to set this to something sane, or
		 * investigate a better way to set the length of output parameters in
		 * the drivers themselves */
		param->max_value_len = Z_STRLEN_P(param->parameter);
	}

	param->stmt = stmt;
	param->is_param = is_param;

	ZVAL_ADDREF(param->parameter);
	if (param->driver_params) {
		ZVAL_ADDREF(param->driver_params);
	}

	if (param->name && stmt->columns) {
		/* try to map the name to the column */
		int i;

		for (i = 0; i < stmt->column_count; i++) {
			if (strcmp(stmt->columns[i].name, param->name) == 0) {
				param->paramno = i;
				break;
			}
		}

		if (param->paramno == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Did not found column name '%s' in the defined columns; it will not be bound", param->name);
		}
	}

	if (param->name) {
		param->name = estrndup(param->name, param->namelen);
		zend_hash_update(hash, param->name, param->namelen, param, sizeof(*param), (void**)&pparam);
	} else {
		zend_hash_index_update(hash, param->paramno, param, sizeof(*param), (void**)&pparam);
	}

	/* tell the driver we just created a parameter */
	if (stmt->methods->param_hook) {
		return stmt->methods->param_hook(stmt, pparam, PDO_PARAM_EVT_ALLOC TSRMLS_CC);
	}

	return 1;
}


/* {{{ proto bool PDOStatement::execute([array $bound_input_params])
   Execute a prepared statement, optionally binding parameters */
static PHP_METHOD(PDOStatement, execute)
{
	pdo_stmt_t *stmt = (pdo_stmt_t*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval *input_params = NULL;
	int ret = 1;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a!", &input_params)) {
		RETURN_FALSE;
	}

	PDO_STMT_CLEAR_ERR();
	
	if (input_params) {
		struct pdo_bound_param_data param;
		zval **tmp;
		uint str_length;
		ulong num_index;

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(input_params));
		while (SUCCESS == zend_hash_get_current_data(Z_ARRVAL_P(input_params), (void*)&tmp)) {
			memset(&param, 0, sizeof(param));

			if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_P(input_params),
						&param.name, &str_length, &num_index, 0, NULL)) {
				param.namelen = str_length;
				param.paramno = -1;
				
				if (!stmt->dbh->placeholders_can_be_strings) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "This driver doesn't support named placeholders");
					RETURN_FALSE;
				}
			} else {
				/* we're okay to be zero based here */
				if (num_index < 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter index");
					RETURN_FALSE;
				}
				param.paramno = num_index;
			}

			param.param_type = PDO_PARAM_STR;
			param.parameter = *tmp;

			if (!really_register_bound_param(&param, stmt, 1 TSRMLS_CC)) {
				RETURN_FALSE;
			}

			zend_hash_move_forward(Z_ARRVAL_P(input_params));
		}
	}

	if (stmt->dbh->emulate_prepare) {
		/* handle the emulated parameter binding,
         * stmt->active_query_string holds the query with binds expanded and 
		 * quoted.
         */
		if(pdo_parse_params(stmt, stmt->query_string, stmt->query_stringlen, 
				&stmt->active_query_string, &stmt->active_query_stringlen) == 0) {
			// parse error in handling the query
			RETURN_FALSE;
		}
	} else if (!dispatch_param_event(stmt, PDO_PARAM_EVT_EXEC_PRE TSRMLS_CC)) {
		RETURN_FALSE;
	}
	if (stmt->methods->executer(stmt TSRMLS_CC)) {
		if (stmt->active_query_string) {
			efree(stmt->active_query_string);
			stmt->active_query_string = NULL;
		}
		if (!stmt->executed) {
			/* this is the first execute */

			if (stmt->dbh->alloc_own_columns) {
				/* for "big boy" drivers, we need to allocate memory to fetch
				 * the results into, so lets do that now */
				ret = describe_columns(stmt TSRMLS_CC);
			}

			stmt->executed = 1;
		}

		if (ret && !dispatch_param_event(stmt, PDO_PARAM_EVT_EXEC_POST TSRMLS_CC)) {
			RETURN_FALSE;
		}
			
		RETURN_BOOL(ret);
	}
	if (stmt->active_query_string) {
		efree(stmt->active_query_string);
		stmt->active_query_string = NULL;
	}
	PDO_HANDLE_STMT_ERR();
	RETURN_FALSE;
}
/* }}} */

static inline void fetch_value(pdo_stmt_t *stmt, zval *dest, int colno TSRMLS_DC)
{
	struct pdo_column_data *col;
	char *value = NULL;
	unsigned long value_len = 0;

	col = &stmt->columns[colno];

	value = NULL;
	value_len = 0;

	stmt->methods->get_col(stmt, colno, &value, &value_len TSRMLS_CC);

	switch (col->param_type) {
		case PDO_PARAM_STR:
			if (value) {
				ZVAL_STRINGL(dest, value, value_len, 1);
				break;
			}
		default:
			ZVAL_NULL(dest);
	}
}

static int do_fetch_common(pdo_stmt_t *stmt, int do_bind TSRMLS_DC)
{
	if (!dispatch_param_event(stmt, PDO_PARAM_EVT_FETCH_PRE TSRMLS_CC)) {
		return 0;
	}

	if (!stmt->methods->fetcher(stmt TSRMLS_CC)) {
		return 0;
	}

	/* some drivers might need to describe the columns now */
	if (!stmt->columns && !describe_columns(stmt TSRMLS_CC)) {
		return 0;
	}
	
	if (!dispatch_param_event(stmt, PDO_PARAM_EVT_FETCH_POST TSRMLS_CC)) {
		return 0;
	}

	if (do_bind && stmt->bound_columns) {
		/* update those bound column variables now */
		struct pdo_bound_param_data *param;

		zend_hash_internal_pointer_reset(stmt->bound_columns);
		while (SUCCESS == zend_hash_get_current_data(stmt->bound_columns, (void**)&param)) {
			if (param->paramno >= 0) {
				convert_to_string(param->parameter);

				/* delete old value */
				zval_dtor(param->parameter);

				/* set new value */
				fetch_value(stmt, param->parameter, param->paramno TSRMLS_CC);

				/* TODO: some smart thing that avoids duplicating the value in the
				 * general loop below.  For now, if you're binding output columns,
				 * it's better to use LAZY or BOUND fetches if you want to shave
				 * off those cycles */
			}

			zend_hash_move_forward(stmt->bound_columns);
		}
	}

	return 1;
}

/* perform a fetch.  If do_bind is true, update any bound columns.
 * If return_value is not null, store values into it according to HOW. */
static int do_fetch(pdo_stmt_t *stmt, int do_bind, zval *return_value, enum pdo_fetch_type how TSRMLS_DC)
{
	if (!do_fetch_common(stmt, do_bind TSRMLS_CC)) {
		return 0;
	}

	if (return_value) {
		int i;

		array_init(return_value);

		if (how == PDO_FETCH_LAZY || how == PDO_FETCH_OBJ) {
			how = PDO_FETCH_BOTH;
		}

		for (i = 0; i < stmt->column_count; i++) {
			zval *val;
			MAKE_STD_ZVAL(val);
			fetch_value(stmt, val, i TSRMLS_CC);

			if (how == PDO_FETCH_ASSOC || how == PDO_FETCH_BOTH) {
				add_assoc_zval(return_value, stmt->columns[i].name, val);
			}
			if (how == PDO_FETCH_NUM || how == PDO_FETCH_BOTH) {
				add_next_index_zval(return_value, val);
			}

			if (how == PDO_FETCH_BOTH) {
				ZVAL_ADDREF(val);
			}
		}
	}

	return 1;
}

/* {{{ proto mixed PDOStatement::fetch([int $how = PDO_FETCH_BOTH])
   Fetches the next row and returns it, or false if there are no more rows */
static PHP_METHOD(PDOStatement, fetch)
{
	long how = PDO_FETCH_BOTH;
	pdo_stmt_t *stmt = (pdo_stmt_t*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &how)) {
		RETURN_FALSE;
	}

	PDO_STMT_CLEAR_ERR();
	if (!do_fetch(stmt, TRUE, return_value, how TSRMLS_CC)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string PDOStatement::fetchSingle()
   Returns a data of the 1st column in the result set. */
static PHP_METHOD(PDOStatement, fetchSingle)
{
	pdo_stmt_t *stmt = (pdo_stmt_t*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS()) {
		RETURN_FALSE;
	}

	PDO_STMT_CLEAR_ERR();
	if (!do_fetch_common(stmt, TRUE TSRMLS_CC)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}

	fetch_value(stmt, return_value, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto array PDOStatement::fetchAll([int $how = PDO_FETCH_BOTH])
   Returns an array of all of the results. */
static PHP_METHOD(PDOStatement, fetchAll)
{
	pdo_stmt_t *stmt = (pdo_stmt_t*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long how = PDO_FETCH_BOTH;
	zval *data;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &how)) {
		RETURN_FALSE;
	}
	
	PDO_STMT_CLEAR_ERR();
	MAKE_STD_ZVAL(data);
	if (!do_fetch(stmt, TRUE, data, how TSRMLS_CC)) {
		PDO_HANDLE_STMT_ERR();
		RETURN_FALSE;
	}

	array_init(return_value);
	do {
		add_next_index_zval(return_value, data);
		MAKE_STD_ZVAL(data);
	} while (do_fetch(stmt, TRUE, data, how TSRMLS_CC));
}
/* }}} */

static int register_bound_param(INTERNAL_FUNCTION_PARAMETERS, pdo_stmt_t *stmt, int is_param)
{
	struct pdo_bound_param_data param = {0};

	param.paramno = -1;
	param.param_type = PDO_PARAM_STR;

	if (stmt->dbh->placeholders_can_be_strings || !is_param) {
		if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					ZEND_NUM_ARGS() TSRMLS_CC, "sz|llz!",
					&param.name, &param.namelen, &param.parameter, &param.param_type,
					&param.max_value_len,
					&param.driver_params)) {
			if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|llz!", &param.paramno,
					&param.parameter, &param.param_type, &param.max_value_len, &param.driver_params)) {
				return 0;
			}	
		}
	} else if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|llz!", &param.paramno,
			&param.parameter, &param.param_type, &param.max_value_len, &param.driver_params)) {
		return 0;
	}

	if (param.paramno > 0) {
		--param.paramno; /* make it zero-based internally */
	} else if (!param.name) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter number: columns are 1-based");
		return 0;
	}

	return really_register_bound_param(&param, stmt, is_param TSRMLS_CC);
}

/* {{{ proto bool PDOStatement::bindParam(mixed $paramno, mixed &$param [, int $type [, int $maxlen [, mixed $driverdata]]])
   bind a parameter to a PHP variable.  $paramno is the 1-based position of the placeholder in the SQL statement (but can be the parameter name for drivers that support named placeholders).  This isn't supported by all drivers.  It should be called prior to execute(). */
static PHP_METHOD(PDOStatement, bindParam)
{
	pdo_stmt_t *stmt = (pdo_stmt_t*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!stmt->dbh->supports_placeholders) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This driver doesn't support placeholders");
		RETURN_FALSE;
	}

	RETURN_BOOL(register_bound_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, stmt, TRUE));
	
}
/* }}} */

/* {{{ proto bool PDOStatement::bindColumn(mixed $column, mixed &$param [, int $type [, int $maxlen [, mixed $driverdata]]])
   bind a column to a PHP variable.  On each row fetch $param will contain the value of the corresponding column.  $column is the 1-based offset of the column, or the column name.  For portability, don't call this before execute(). */
static PHP_METHOD(PDOStatement, bindColumn)
{
	pdo_stmt_t *stmt = (pdo_stmt_t*)zend_object_store_get_object(getThis() TSRMLS_CC);
	RETURN_BOOL(register_bound_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, stmt, FALSE));
}
/* }}} */

/* {{{ proto int PDOStatement::rowCount()
   Returns the number of rows in a result set, or the number of rows affected by the last execute().  It is not always meaningful. */
static PHP_METHOD(PDOStatement, rowCount)
{
	pdo_stmt_t *stmt = (pdo_stmt_t*)zend_object_store_get_object(getThis() TSRMLS_CC);

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "This statement is not a scrollable cursor and does not know the row count");
	RETURN_FALSE;
}
/* }}} */



function_entry pdo_dbstmt_functions[] = {
	PHP_ME(PDOStatement, execute,		NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDOStatement, fetch,			NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDOStatement, bindParam,		second_arg_force_ref,	ZEND_ACC_PUBLIC)
	PHP_ME(PDOStatement, bindColumn,	second_arg_force_ref,	ZEND_ACC_PUBLIC)
	PHP_ME(PDOStatement, rowCount,		NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDOStatement, fetchSingle,		NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDOStatement, fetchAll,		NULL,					ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ overloaded handlers for PDOStatement class */
static zval *dbstmt_prop_read(zval *object, zval *member, int type TSRMLS_DC)
{
	zval *return_value;
	pdo_stmt_t * stmt = (pdo_stmt_t *) zend_object_store_get_object(object TSRMLS_CC);

	convert_to_string(member);

	if(strcmp(Z_STRVAL_P(member), "queryString") == 0) {
		MAKE_STD_ZVAL(return_value);
		ZVAL_STRINGL(return_value, stmt->query_string, stmt->query_stringlen, 1);
	}
	else {
		MAKE_STD_ZVAL(return_value);
		ZVAL_NULL(return_value);
	}
	return return_value;
}

static void dbstmt_prop_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	return;
}

static zval *dbstmt_read_dim(zval *object, zval *offset, int type TSRMLS_DC)
{
	zval *return_value;

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	return return_value;
}

static void dbstmt_write_dim(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	return;
}

static int dbstmt_prop_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	return 0;
}

static int dbstmt_dim_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	return 0;
}

static void dbstmt_prop_delete(zval *object, zval *offset TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete properties from a PDO STMT");
}

static void dbstmt_dim_delete(zval *object, zval *offset TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete dimensions from a PDO STMT");
}

static HashTable *dbstmt_get_properties(zval *object TSRMLS_DC)
{
	return NULL;
}

static union _zend_function *dbstmt_method_get(zval *object, char *method_name, int method_len TSRMLS_DC)
{
	zend_function *fbc;
	char *lc_method_name;

	lc_method_name = do_alloca(method_len + 1);
	zend_str_tolower_copy(lc_method_name, method_name, method_len);

	if (zend_hash_find(&pdo_dbstmt_ce->function_table, lc_method_name, method_len+1, (void**)&fbc) == FAILURE) {
		free_alloca(lc_method_name);
		return NULL;
	}
	
	free_alloca(lc_method_name);
	return fbc;
}

static int dbstmt_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	return FAILURE;
}

static union _zend_function *dbstmt_get_ctor(zval *object TSRMLS_DC)
{
	static zend_internal_function ctor = {0};

	ctor.type = ZEND_INTERNAL_FUNCTION;
	ctor.function_name = "__construct";
	ctor.scope = pdo_dbstmt_ce;
	ctor.handler = ZEND_FN(dbstmt_constructor);

	return (union _zend_function*)&ctor;
}

static zend_class_entry *dbstmt_get_ce(zval *object TSRMLS_DC)
{
	return pdo_dbstmt_ce;
}

static int dbstmt_get_classname(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	*class_name = estrndup("PDOStatement", sizeof("PDOStatement")-1);
	*class_name_len = sizeof("PDOStatement")-1;
	return 0;
}

static int dbstmt_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	return -1;
}

zend_object_handlers pdo_dbstmt_object_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	dbstmt_prop_read,
	dbstmt_prop_write,
	dbstmt_read_dim,
	dbstmt_write_dim,
	NULL,
	NULL,
	NULL,
	dbstmt_prop_exists,
	dbstmt_prop_delete,
	dbstmt_dim_exists,
	dbstmt_dim_delete,
	dbstmt_get_properties,
	dbstmt_method_get,
	dbstmt_call_method,
	dbstmt_get_ctor,
	dbstmt_get_ce,
	dbstmt_get_classname,
	dbstmt_compare,
	NULL, /* cast */
	NULL
};

void pdo_dbstmt_free_storage(zend_object *object TSRMLS_DC)
{
	pdo_stmt_t *stmt = (pdo_stmt_t*)object;

	if (stmt->methods && stmt->methods->dtor) {
		stmt->methods->dtor(stmt TSRMLS_CC);
	}
	if(stmt->query_string) {
		efree(stmt->query_string);
	}
	zend_objects_store_del_ref(&stmt->database_object_handle TSRMLS_CC);
		
	efree(stmt);
}

zend_object_value pdo_dbstmt_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;

	retval.handle = zend_objects_store_put(NULL, NULL, pdo_dbstmt_free_storage, NULL TSRMLS_CC);
	retval.handlers = &pdo_dbstmt_object_handlers;

	return retval;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
