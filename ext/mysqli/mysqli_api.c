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
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "zend_smart_str.h"
#include "php_mysqli_structs.h"
#include "mysqli_priv.h"
#include "ext/mysqlnd/mysql_float_to_double.h"

#define ERROR_ARG_POS(arg_num) (getThis() ? (arg_num-1) : (arg_num))

/* {{{ Get number of affected rows in previous MySQL operation */
PHP_FUNCTION(mysqli_affected_rows)
{
	MY_MYSQL 		*mysql;
	zval  			*mysql_link;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	rc = mysql_affected_rows(mysql->mysql);
	if (rc == (my_ulonglong) -1) {
		RETURN_LONG(-1);
	}
	MYSQLI_RETURN_LONG_INT(rc);
}
/* }}} */

/* {{{ Turn auto commit on or of */
PHP_FUNCTION(mysqli_autocommit)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	bool	automode;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ob", &mysql_link, mysqli_link_class_entry, &automode) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_autocommit(mysql->mysql, (my_bool)automode)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ mysqli_stmt_bind_param_do_bind */
static
int mysqli_stmt_bind_param_do_bind(MY_STMT *stmt, unsigned int num_vars, zval *args, const char * const types, unsigned int num_extra_args)
{
	unsigned int i;
	MYSQLND_PARAM_BIND	*params;
	enum_func_status	ret = FAIL;

	/* If no params -> skip binding and return directly */
	if (num_vars == 0) {
		return PASS;
	}
	params = mysqlnd_stmt_alloc_param_bind(stmt->stmt);
	if (!params) {
		goto end;
	}
	for (i = 0; i < num_vars; i++) {
		uint8_t type;
		switch (types[i]) {
			case 'd': /* Double */
				type = MYSQL_TYPE_DOUBLE;
				break;
			case 'i': /* Integer */
#if SIZEOF_ZEND_LONG==8
				type = MYSQL_TYPE_LONGLONG;
#elif SIZEOF_ZEND_LONG==4
				type = MYSQL_TYPE_LONG;
#endif
				break;
			case 'b': /* Blob (send data) */
				type = MYSQL_TYPE_LONG_BLOB;
				break;
			case 's': /* string */
				type = MYSQL_TYPE_VAR_STRING;
				break;
			default:
				zend_argument_value_error(num_extra_args, "must only contain the \"b\", \"d\", \"i\", \"s\" type specifiers");
				ret = FAIL;
				mysqlnd_stmt_free_param_bind(stmt->stmt, params);
				goto end;
		}
		ZVAL_COPY_VALUE(&params[i].zv, &args[i]);
		params[i].type = type;
	}
	ret = mysqlnd_stmt_bind_param(stmt->stmt, params);

end:
	return ret;
}
/* }}} */

/* {{{ Bind variables to a prepared statement as parameters */
PHP_FUNCTION(mysqli_stmt_bind_param)
{
	zval			*args;
	int				argc;
	MY_STMT			*stmt;
	zval			*mysql_stmt;
	char			*types;
	size_t			types_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os*", &mysql_stmt, mysqli_stmt_class_entry, &types, &types_len, &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (!types_len) {
		zend_argument_value_error(ERROR_ARG_POS(2), "cannot be empty");
		RETURN_THROWS();
	}

	if (types_len != (size_t) argc) {
		/* number of bind variables doesn't match number of elements in type definition string */
		zend_argument_count_error("The number of elements in the type definition string must match the number of bind variables");
		RETURN_THROWS();
	}

	if (types_len != mysql_stmt_param_count(stmt->stmt)) {
		zend_argument_count_error("The number of variables must match the number of parameters in the prepared statement");
		RETURN_THROWS();
	}

	RETVAL_BOOL(!mysqli_stmt_bind_param_do_bind(stmt, argc, args, types, getThis() ? 1 : 2));
	MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
}
/* }}} */

/* {{{ mysqli_stmt_bind_result_do_bind */
static int
mysqli_stmt_bind_result_do_bind(MY_STMT *stmt, zval *args, unsigned int argc)
{
	unsigned int i;
	MYSQLND_RESULT_BIND *params = mysqlnd_stmt_alloc_result_bind(stmt->stmt);
	if (params) {
		for (i = 0; i < argc; i++) {
			ZVAL_COPY_VALUE(&params[i].zv, &args[i]);
		}
		return mysqlnd_stmt_bind_result(stmt->stmt, params);
	}
	return FAIL;
}
/* }}} */

/* {{{ Bind variables to a prepared statement for result storage */
PHP_FUNCTION(mysqli_stmt_bind_result)
{
	zval		*args;
	int			argc;
	zend_ulong		rc;
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O+", &mysql_stmt, mysqli_stmt_class_entry, &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if ((uint32_t)argc != mysql_stmt_field_count(stmt->stmt)) {
		zend_argument_count_error("Number of bind variables doesn't match number of fields in prepared statement");
		RETURN_THROWS();
	}

	rc = mysqli_stmt_bind_result_do_bind(stmt, args, argc);
	RETURN_BOOL(!rc);
}
/* }}} */

/* {{{ Change logged-in user of the active connection */
PHP_FUNCTION(mysqli_change_user)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;
	char		*user, *password, *dbname;
	size_t			user_len, password_len, dbname_len;
	zend_ulong		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osss!", &mysql_link, mysqli_link_class_entry, &user, &user_len, &password, &password_len, &dbname, &dbname_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	rc = mysqlnd_change_user_ex(mysql->mysql, user, password, dbname, false, (size_t) password_len);
	MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);

	if (rc) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns the name of the character set used for this connection */
PHP_FUNCTION(mysqli_character_set_name)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	RETURN_STRING(mysql_character_set_name(mysql->mysql));
}
/* }}} */

/* {{{ php_mysqli_close */
void php_mysqli_close(MY_MYSQL * mysql, int close_type, int resource_status)
{
	if (resource_status > MYSQLI_STATUS_INITIALIZED) {
		MyG(num_links)--;
	}

	if (!mysql->persistent) {
		mysqli_close(mysql->mysql, close_type);
	} else {
		zend_resource *le;
		if ((le = zend_hash_find_ptr(&EG(persistent_list), mysql->hash_key)) != NULL) {
			if (le->type == php_le_pmysqli()) {
				mysqli_plist_entry *plist = (mysqli_plist_entry *) le->ptr;
				mysqlnd_end_psession(mysql->mysql);

				if (MyG(rollback_on_cached_plink) &&
					FAIL == mysqlnd_rollback(mysql->mysql, TRANS_COR_NO_OPT, NULL))
				{
					mysqli_close(mysql->mysql, close_type);
				} else {
					zend_ptr_stack_push(&plist->free_links, mysql->mysql);
					MyG(num_inactive_persistent)++;
				}
				MyG(num_active_persistent)--;
			}
		}
		mysql->persistent = false;
	}
	mysql->mysql = NULL;

	php_clear_mysql(mysql);
}
/* }}} */

/* {{{ Close connection */
PHP_FUNCTION(mysqli_close)
{
	zval		*mysql_link;
	MY_MYSQL	*mysql;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_INITIALIZED);

	php_mysqli_close(mysql, MYSQLI_CLOSE_EXPLICIT, ((MYSQLI_RESOURCE *)(Z_MYSQLI_P(mysql_link))->ptr)->status);
	((MYSQLI_RESOURCE *)(Z_MYSQLI_P(mysql_link))->ptr)->status = MYSQLI_STATUS_UNKNOWN;

	MYSQLI_CLEAR_RESOURCE(mysql_link);
	efree(mysql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Commit outstanding actions and close transaction */
PHP_FUNCTION(mysqli_commit)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_long		flags = TRANS_COR_NO_OPT;
	char *		name = NULL;
	size_t			name_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|ls!", &mysql_link, mysqli_link_class_entry, &flags, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (FAIL == mysqlnd_commit(mysql->mysql, flags, name)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Move internal result pointer */
PHP_FUNCTION(mysqli_data_seek)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	zend_long		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &offset) == FAILURE) {
		RETURN_THROWS();
	}

	if (offset < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (mysqli_result_is_unbuffered(result)) {
		if (getThis()) {
			zend_throw_error(NULL, "mysqli_result::data_seek() cannot be used in MYSQLI_USE_RESULT mode");
		} else {
			zend_throw_error(NULL, "mysqli_data_seek() cannot be used in MYSQLI_USE_RESULT mode");
		}
		RETURN_THROWS();
	}

	if ((uint64_t)offset >= mysql_num_rows(result)) {
		RETURN_FALSE;
	}

	mysql_data_seek(result, offset);
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_debug)
{
	char	*debug;
	size_t		debug_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &debug, &debug_len) == FAILURE) {
		RETURN_THROWS();
	}

	mysql_debug(debug);
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_dump_debug_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	RETURN_BOOL(!mysql_dump_debug_info(mysql->mysql));
}
/* }}} */

/* {{{ Returns the numerical value of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_errno)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	RETURN_LONG(mysql_errno(mysql->mysql));
}
/* }}} */

/* {{{ Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_error)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	RETURN_STRING(mysql_error(mysql->mysql));
}
/* }}} */

/* {{{ Execute a prepared statement */
PHP_FUNCTION(mysqli_stmt_execute)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;
	HashTable	*input_params = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|h!", &mysql_stmt, mysqli_stmt_class_entry, &input_params) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	// bind-in-execute
	if (input_params) {
		zval *tmp;
		unsigned int index;
		unsigned int hash_num_elements;
		unsigned int param_count;
		MYSQLND_PARAM_BIND	*params;

		if (!zend_array_is_list(input_params)) {
			zend_argument_value_error(ERROR_ARG_POS(2), "must be a list array");
			RETURN_THROWS();
		}

		hash_num_elements = zend_hash_num_elements(input_params);
		param_count = mysql_stmt_param_count(stmt->stmt);
		if (hash_num_elements != param_count) {
			zend_argument_value_error(ERROR_ARG_POS(2), "must consist of exactly %d elements, %d present", param_count, hash_num_elements);
			RETURN_THROWS();
		}

		params = mysqlnd_stmt_alloc_param_bind(stmt->stmt);
		ZEND_ASSERT(params);

		index = 0;
		ZEND_HASH_FOREACH_VAL(input_params, tmp) {
			ZVAL_COPY_VALUE(&params[index].zv, tmp);
			params[index].type = MYSQL_TYPE_VAR_STRING;
			index++;
		} ZEND_HASH_FOREACH_END();

		if (mysqlnd_stmt_bind_param(stmt->stmt, params)) {
			MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
			RETVAL_FALSE;
		}
	}

	if (mysql_stmt_execute(stmt->stmt)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(stmt->query, mysqli_stmt_server_status(stmt->stmt));
	}
}
/* }}} */

void close_stmt_and_copy_errors(MY_STMT *stmt, MY_MYSQL *mysql)
{
	/* mysql_stmt_close() clears errors, so we have to store them temporarily */
	MYSQLND_ERROR_INFO error_info = *stmt->stmt->data->error_info;
	stmt->stmt->data->error_info->error_list.head = NULL;
	stmt->stmt->data->error_info->error_list.tail = NULL;
	stmt->stmt->data->error_info->error_list.count = 0;

	/* we also remember affected_rows which gets cleared too */
	uint64_t affected_rows = mysql->mysql->data->upsert_status->affected_rows;

	mysqli_stmt_close(stmt->stmt, false);
	stmt->stmt = NULL;
	php_clear_stmt_bind(stmt);

	/* restore error messages, but into the mysql object */
	zend_llist_clean(&mysql->mysql->data->error_info->error_list);
	*mysql->mysql->data->error_info = error_info;
	mysql->mysql->data->upsert_status->affected_rows = affected_rows;
}

PHP_FUNCTION(mysqli_execute_query)
{
	MY_MYSQL		*mysql;
	MY_STMT			*stmt;
	char			*query = NULL;
	size_t				query_len;
	zval			*mysql_link;
	HashTable	*input_params = NULL;
	MYSQL_RES 		*result;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|h!", &mysql_link, mysqli_link_class_entry, &query, &query_len, &input_params) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	stmt = (MY_STMT *)ecalloc(1,sizeof(MY_STMT));

	if (!(stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		efree(stmt);
		RETURN_FALSE;
	}

	if (FAIL == mysql_stmt_prepare(stmt->stmt, query, query_len)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		
		close_stmt_and_copy_errors(stmt, mysql);
		RETURN_FALSE;
	}

	/* The bit below, which is copied from mysqli_prepare, is needed for bad index exceptions */ 
	/* don't initialize stmt->query with NULL, we ecalloc()-ed the memory */
	/* Get performance boost if reporting is switched off */
	if (query_len && (MyG(report_mode) & MYSQLI_REPORT_INDEX)) {
		stmt->query = estrdup(query);
	}

	// bind-in-execute
	// It's very similar to the mysqli_stmt::execute, but it uses different error handling
	if (input_params) {
		zval *tmp;
		unsigned int index;
		unsigned int hash_num_elements;
		unsigned int param_count;
		MYSQLND_PARAM_BIND	*params;

		if (!zend_array_is_list(input_params)) {
			mysqli_stmt_close(stmt->stmt, false);
			stmt->stmt = NULL;
			efree(stmt);
			zend_argument_value_error(ERROR_ARG_POS(3), "must be a list array");
			RETURN_THROWS();
		}

		hash_num_elements = zend_hash_num_elements(input_params);
		param_count = mysql_stmt_param_count(stmt->stmt);
		if (hash_num_elements != param_count) {
			mysqli_stmt_close(stmt->stmt, false);
			stmt->stmt = NULL;
			efree(stmt);
			zend_argument_value_error(ERROR_ARG_POS(3), "must consist of exactly %d elements, %d present", param_count, hash_num_elements);
			RETURN_THROWS();
		}

		params = mysqlnd_stmt_alloc_param_bind(stmt->stmt);
		ZEND_ASSERT(params);

		index = 0;
		ZEND_HASH_FOREACH_VAL(input_params, tmp) {
			ZVAL_COPY_VALUE(&params[index].zv, tmp);
			params[index].type = MYSQL_TYPE_VAR_STRING;
			index++;
		} ZEND_HASH_FOREACH_END();

		if (mysqlnd_stmt_bind_param(stmt->stmt, params)) {
			close_stmt_and_copy_errors(stmt, mysql);
			RETURN_FALSE;
		}

	}

	if (mysql_stmt_execute(stmt->stmt)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);

		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
			php_mysqli_report_index(stmt->query, mysqli_stmt_server_status(stmt->stmt));
		}

		close_stmt_and_copy_errors(stmt, mysql);
		RETURN_FALSE;
	}

	if (!mysql_stmt_field_count(stmt->stmt)) {
		/* no result set - not a SELECT */
		close_stmt_and_copy_errors(stmt, mysql);
		RETURN_TRUE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(stmt->query, mysqli_stmt_server_status(stmt->stmt));
	}

	/* get result */
	if (!(result = mysqlnd_stmt_get_result(stmt->stmt))) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);

		close_stmt_and_copy_errors(stmt, mysql);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_result_class_entry);

	close_stmt_and_copy_errors(stmt, mysql);
}

/* {{{ mixed mysqli_stmt_fetch_mysqlnd */
void mysqli_stmt_fetch_mysqlnd(INTERNAL_FUNCTION_PARAMETERS)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;
	bool	fetched_anything;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (FAIL == mysqlnd_stmt_fetch(stmt->stmt, &fetched_anything)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	} else if (fetched_anything) {
		RETURN_TRUE;
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ Fetch results from a prepared statement into the bound variables */
PHP_FUNCTION(mysqli_stmt_fetch)
{
	mysqli_stmt_fetch_mysqlnd(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{  php_add_field_properties */
static void php_add_field_properties(zval *value, const MYSQL_FIELD *field)
{
	add_property_str(value, "name", zend_string_copy(field->sname));

	add_property_stringl(value, "orgname", (field->org_name ? field->org_name : ""), field->org_name_length);
	add_property_stringl(value, "table", (field->table ? field->table : ""), field->table_length);
	add_property_stringl(value, "orgtable", (field->org_table ? field->org_table : ""), field->org_table_length);
	add_property_stringl(value, "def", (field->def ? field->def : ""), field->def_length);
	add_property_stringl(value, "db", (field->db ? field->db : ""), field->db_length);

	/* FIXME: manually set the catalog to "def" due to bug in
	 * libmysqlclient which does not initialize field->catalog
	 * and in addition, the catalog is always be "def"
	 */
	add_property_string(value, "catalog", "def");

	add_property_long(value, "max_length", 0);
	add_property_long(value, "length", field->length);
	add_property_long(value, "charsetnr", field->charsetnr);
	add_property_long(value, "flags", field->flags);
	add_property_long(value, "type", field->type);
	add_property_long(value, "decimals", field->decimals);
}
/* }}} */

/* {{{ Get column information from a result and return as an object */
PHP_FUNCTION(mysqli_fetch_field)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	const MYSQL_FIELD	*field;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (!(field = mysql_fetch_field(result))) {
		RETURN_FALSE;
	}

	object_init(return_value);
	php_add_field_properties(return_value, field);
}
/* }}} */

/* {{{ Return array of objects containing field meta-data */
PHP_FUNCTION(mysqli_fetch_fields)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	zval		obj;

	unsigned int i, num_fields;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	array_init(return_value);
	num_fields = mysql_num_fields(result);

	for (i = 0; i < num_fields; i++) {
		const MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);

		object_init(&obj);

		php_add_field_properties(&obj, field);
		add_index_zval(return_value, i, &obj);
	}
}
/* }}} */

/* {{{ Fetch meta-data for a single field */
PHP_FUNCTION(mysqli_fetch_field_direct)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	const MYSQL_FIELD	*field;
	zend_long		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &offset) == FAILURE) {
		RETURN_THROWS();
	}

	if (offset < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (offset >= (zend_long) mysql_num_fields(result)) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be less than the number of fields for this result set");
		RETURN_THROWS();
	}

	if (!(field = mysql_fetch_field_direct(result,offset))) {
		RETURN_FALSE;
	}

	object_init(return_value);
	php_add_field_properties(return_value, field);
}
/* }}} */

/* {{{ Get the length of each output in a result */
PHP_FUNCTION(mysqli_fetch_lengths)
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	unsigned int	i, num_fields;
	const size_t	*ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	// TODO Warning?
	if (!(ret = mysql_fetch_lengths(result))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	num_fields = mysql_num_fields(result);

	for (i = 0; i < num_fields; i++) {
		add_index_long(return_value, i, ret[i]);
	}
}
/* }}} */

/* {{{ Get a result row as an enumerated array */
PHP_FUNCTION(mysqli_fetch_row)
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_NUM, 0);
}
/* }}} */

/* {{{ Fetch the number of fields returned by the last query for the given link */
PHP_FUNCTION(mysqli_field_count)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_field_count(mysql->mysql));
}
/* }}} */

/* {{{ Set result pointer to a specified field offset */
PHP_FUNCTION(mysqli_field_seek)
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	zend_long	fieldnr;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &mysql_result, mysqli_result_class_entry, &fieldnr) == FAILURE) {
		RETURN_THROWS();
	}

	if (fieldnr < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if ((uint32_t)fieldnr >= mysql_num_fields(result)) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be less than the number of fields for this result set");
		RETURN_THROWS();
	}

	mysql_field_seek(result, fieldnr);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get current field offset of result pointer */
PHP_FUNCTION(mysqli_field_tell)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_field_tell(result));
}
/* }}} */

/* {{{ Free query result memory for the given result handle */
PHP_FUNCTION(mysqli_free_result)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	mysqli_free_result(result, false);
	MYSQLI_CLEAR_RESOURCE(mysql_result);
}
/* }}} */

/* {{{ Get MySQL client info */
PHP_FUNCTION(mysqli_get_client_info)
{
	if (getThis()) {
		if (zend_parse_parameters_none() == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		zval *mysql_link;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!", &mysql_link, mysqli_link_class_entry) == FAILURE) {
			RETURN_THROWS();
		}

		if (ZEND_NUM_ARGS()) {
			php_error_docref(NULL, E_DEPRECATED, "Passing connection object as an argument is deprecated");
		}
	}

	RETURN_STRING(mysql_get_client_info());
}
/* }}} */

/* {{{ Get MySQL client info */
PHP_FUNCTION(mysqli_get_client_version)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG((zend_long)mysql_get_client_version());
}
/* }}} */

/* {{{ Get MySQL host info */
PHP_FUNCTION(mysqli_get_host_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	RETURN_STRING((mysql->mysql->data->host_info) ? mysql->mysql->data->host_info : "");
}
/* }}} */

/* {{{ Get MySQL protocol information */
PHP_FUNCTION(mysqli_get_proto_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	RETURN_LONG(mysql_get_proto_info(mysql->mysql));
}
/* }}} */

/* {{{ Get MySQL server info */
PHP_FUNCTION(mysqli_get_server_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	RETURN_STRING(mysql_get_server_info(mysql->mysql));
}
/* }}} */

/* {{{ Return the MySQL version for the server referenced by the given link */
PHP_FUNCTION(mysqli_get_server_version)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_get_server_version(mysql->mysql));
}
/* }}} */

/* {{{ Get information about the most recent query */
PHP_FUNCTION(mysqli_info)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;
	const char	*info;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	info = mysql_info(mysql->mysql);
	if (info) {
		RETURN_STRING(info);
	}
}
/* }}} */

/* {{{ php_mysqli_init() */
void php_mysqli_init(INTERNAL_FUNCTION_PARAMETERS, bool is_method)
{
	MYSQLI_RESOURCE *mysqli_resource;
	MY_MYSQL *mysql;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (is_method && (Z_MYSQLI_P(getThis()))->ptr) {
		return;
	}

	mysql = (MY_MYSQL *)ecalloc(1, sizeof(MY_MYSQL));

	/*
	  We create always persistent, as if the user want to connect
	  to p:somehost, we can't convert the handle then
	*/
	if (!(mysql->mysql = mysqlnd_init(MYSQLND_CLIENT_NO_FLAG, true)))
	{
		efree(mysql);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)mysql;
	mysqli_resource->status = MYSQLI_STATUS_INITIALIZED;

	if (!is_method) {
		MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_link_class_entry);
	} else {
		(Z_MYSQLI_P(getThis()))->ptr = mysqli_resource;
	}
}
/* }}} */

/* {{{ Initialize mysqli and return a resource for use with mysql_real_connect */
PHP_FUNCTION(mysqli_init)
{
	php_mysqli_init(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} */

/* {{{ Get the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysqli_insert_id)
{
	MY_MYSQL		*mysql;
	my_ulonglong	rc;
	zval			*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	rc = mysql_insert_id(mysql->mysql);
	MYSQLI_RETURN_LONG_INT(rc)
}
/* }}} */

/* {{{ Kill a mysql process on the server */
PHP_FUNCTION(mysqli_kill)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_long		processid;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &mysql_link, mysqli_link_class_entry, &processid) == FAILURE) {
		RETURN_THROWS();
	}

	if (processid <= 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than 0");
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_kill(mysql->mysql, processid)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ check if there any more query results from a multi query */
PHP_FUNCTION(mysqli_more_results)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	RETURN_BOOL(mysql_more_results(mysql->mysql));
}
/* }}} */

/* {{{ read next result from multi_query */
PHP_FUNCTION(mysqli_next_result) {
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_next_result(mysql->mysql)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ check if there any more query results from a multi query */
PHP_FUNCTION(mysqli_stmt_more_results)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	RETURN_BOOL(mysqlnd_stmt_more_results(stmt->stmt));
}
/* }}} */

/* {{{ read next result from multi_query */
PHP_FUNCTION(mysqli_stmt_next_result) {
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (mysql_stmt_next_result(stmt->stmt)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get number of fields in result */
PHP_FUNCTION(mysqli_num_fields)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_num_fields(result));
}
/* }}} */

/* {{{ Get number of rows in result */
PHP_FUNCTION(mysqli_num_rows)
{
	MYSQL_RES	*result;
	zval		*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (mysqli_result_is_unbuffered_and_not_everything_is_fetched(result)) {
		zend_throw_error(NULL, "mysqli_num_rows() cannot be used in MYSQLI_USE_RESULT mode");
		RETURN_THROWS();
	}

	MYSQLI_RETURN_LONG_INT(mysql_num_rows(result));
}
/* }}} */

/* {{{ mysqli_options_get_option_zval_type */
static int mysqli_options_get_option_zval_type(int option)
{
	switch (option) {
		case MYSQLND_OPT_NET_CMD_BUFFER_SIZE:
		case MYSQLND_OPT_NET_READ_BUFFER_SIZE:
		case MYSQLND_OPT_INT_AND_FLOAT_NATIVE:
		case MYSQL_OPT_CONNECT_TIMEOUT:
#ifdef MYSQL_REPORT_DATA_TRUNCATION
		case MYSQL_REPORT_DATA_TRUNCATION:
#endif
		case MYSQL_OPT_LOCAL_INFILE:
		case MYSQL_OPT_NAMED_PIPE:
#ifdef MYSQL_OPT_PROTOCOL
		case MYSQL_OPT_PROTOCOL:
#endif /* MySQL 4.1.0 */
		case MYSQL_OPT_READ_TIMEOUT:
		case MYSQL_OPT_WRITE_TIMEOUT:
#ifdef MYSQL_OPT_GUESS_CONNECTION /* removed in MySQL-8.0 */
		case MYSQL_OPT_GUESS_CONNECTION:
		case MYSQL_OPT_USE_EMBEDDED_CONNECTION:
		case MYSQL_OPT_USE_REMOTE_CONNECTION:
		case MYSQL_SECURE_AUTH:
#endif
#ifdef MYSQL_OPT_RECONNECT
		case MYSQL_OPT_RECONNECT:
#endif /* MySQL 5.0.13 */
#ifdef MYSQL_OPT_SSL_VERIFY_SERVER_CERT
		case MYSQL_OPT_SSL_VERIFY_SERVER_CERT:
#endif /* MySQL 5.0.23 */
#ifdef MYSQL_OPT_COMPRESS
		case MYSQL_OPT_COMPRESS:
#endif /* mysqlnd @ PHP 5.3.2 */
		case MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS:
			return IS_LONG;

#ifdef MYSQL_SHARED_MEMORY_BASE_NAME
		case MYSQL_SHARED_MEMORY_BASE_NAME:
#endif /* MySQL 4.1.0 */
#ifdef MYSQL_SET_CLIENT_IP
		case MYSQL_SET_CLIENT_IP:
#endif /* MySQL 4.1.1 */
		case MYSQL_READ_DEFAULT_FILE:
		case MYSQL_READ_DEFAULT_GROUP:
		case MYSQL_INIT_COMMAND:
		case MYSQL_SET_CHARSET_NAME:
		case MYSQL_SET_CHARSET_DIR:
		case MYSQL_SERVER_PUBLIC_KEY:
		case MYSQL_OPT_LOAD_DATA_LOCAL_DIR:
			return IS_STRING;

		default:
			return IS_NULL;
	}
}
/* }}} */

/* {{{ Set options */
PHP_FUNCTION(mysqli_options)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link = NULL;
	zval			*mysql_value;
	zend_long			mysql_option;
	unsigned int	l_value;
	zend_long			ret;
	int				expected_type;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Olz", &mysql_link, mysqli_link_class_entry, &mysql_option, &mysql_value) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_INITIALIZED);

	expected_type = mysqli_options_get_option_zval_type(mysql_option);
	if (expected_type != Z_TYPE_P(mysql_value)) {
		switch (expected_type) {
			case IS_STRING:
				if (!try_convert_to_string(mysql_value)) {
					RETURN_THROWS();
				}
				break;
			case IS_LONG:
				convert_to_long(mysql_value);
				break;
			default:
				break;
		}
	}
	switch (expected_type) {
		case IS_STRING:
			if ((ret = mysql_options(mysql->mysql, mysql_option, Z_STRVAL_P(mysql_value)))) {
				MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
			}
			break;
		case IS_LONG:
			l_value = Z_LVAL_P(mysql_value);
			if ((ret = mysql_options(mysql->mysql, mysql_option, (char *)&l_value))) {
				MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
			}
			break;
		default:
			ret = 1;
			break;
	}

	RETURN_BOOL(!ret);
}
/* }}} */

/* {{{ Ping a server connection or reconnect if there is no connection */
PHP_FUNCTION(mysqli_ping)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_long		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	rc = mysql_ping(mysql->mysql);
	MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);

	RETURN_BOOL(!rc);
}
/* }}} */

/* {{{ Prepare a SQL statement for execution */
PHP_FUNCTION(mysqli_prepare)
{
	MY_MYSQL		*mysql;
	MY_STMT			*stmt;
	char			*query = NULL;
	size_t				query_len;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os",&mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	stmt = (MY_STMT *)ecalloc(1,sizeof(MY_STMT));

	if ((stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		if (mysql_stmt_prepare(stmt->stmt, query, query_len)) {
			/* mysql_stmt_close() clears errors, so we have to store them temporarily */
			MYSQLND_ERROR_INFO error_info = *mysql->mysql->data->error_info;
			mysql->mysql->data->error_info->error_list.head = NULL;
			mysql->mysql->data->error_info->error_list.tail = NULL;
			mysql->mysql->data->error_info->error_list.count = 0;
			mysqli_stmt_close(stmt->stmt, false);
			stmt->stmt = NULL;

			/* restore error messages */
			zend_llist_clean(&mysql->mysql->data->error_info->error_list);
			*mysql->mysql->data->error_info = error_info;
		}
	}

	/* don't initialize stmt->query with NULL, we ecalloc()-ed the memory */
	/* Get performance boost if reporting is switched off */
	if (stmt->stmt && query_len && (MyG(report_mode) & MYSQLI_REPORT_INDEX)) {
		stmt->query = estrdup(query);
	}

	/* don't join to the previous if because it won't work if mysql_stmt_prepare_fails */
	if (!stmt->stmt) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)stmt;

	/* change status */
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_stmt_class_entry);
}
/* }}} */

/* {{{ Open a connection to a mysql server */
PHP_FUNCTION(mysqli_real_connect)
{
	mysqli_common_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, true, false);
}
/* }}} */

/* {{{ Binary-safe version of mysql_query() */
PHP_FUNCTION(mysqli_real_query)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*query = NULL;
	size_t		query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	MYSQLI_DISABLE_MQ; /* disable multi statements/queries */

	if (mysql_real_query(mysql->mysql, query, query_len)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql->mysql)) {
		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
			php_mysqli_report_index(query, mysqli_server_status(mysql->mysql));
		}
	}

	RETURN_TRUE;
}
/* }}} */

# define mysql_real_escape_string_quote(mysql, to, from, length, quote) \
	mysql_real_escape_string(mysql, to, from, length)

PHP_FUNCTION(mysqli_real_escape_string) {
	MY_MYSQL	*mysql;
	zval		*mysql_link = NULL;
	char		*escapestr;
	size_t			escapestr_len;
	zend_string *newstr;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &escapestr, &escapestr_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	newstr = zend_string_safe_alloc(2, escapestr_len, 0, 0);
	ZSTR_LEN(newstr) = mysql_real_escape_string_quote(mysql->mysql, ZSTR_VAL(newstr), escapestr, escapestr_len, '\'');
	newstr = zend_string_truncate(newstr, ZSTR_LEN(newstr), 0);

	RETURN_NEW_STR(newstr);
}

/* {{{ Undo actions from current transaction */
PHP_FUNCTION(mysqli_rollback)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_long		flags = TRANS_COR_NO_OPT;
	char *		name = NULL;
	size_t			name_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|ls!", &mysql_link, mysqli_link_class_entry, &flags, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);


	if (FAIL == mysqlnd_rollback(mysql->mysql, flags, name)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_send_long_data)
{
	MY_STMT *stmt;
	zval	*mysql_stmt;
	char	*data;
	zend_long	param_nr;
	size_t		data_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ols", &mysql_stmt, mysqli_stmt_class_entry, &param_nr, &data, &data_len) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (param_nr < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (mysql_stmt_send_long_data(stmt->stmt, param_nr, data, data_len)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Return the number of rows affected in the last query for the given link. */
PHP_FUNCTION(mysqli_stmt_affected_rows)
{
	MY_STMT			*stmt;
	zval			*mysql_stmt;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	rc = mysql_stmt_affected_rows(stmt->stmt);
	if (rc == (my_ulonglong) -1) {
		RETURN_LONG(-1);
	}
	MYSQLI_RETURN_LONG_INT(rc)
}
/* }}} */

/* {{{ Close statement */
PHP_FUNCTION(mysqli_stmt_close)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	mysqli_stmt_close(stmt->stmt, false);
	stmt->stmt = NULL;
	php_clear_stmt_bind(stmt);
	MYSQLI_CLEAR_RESOURCE(mysql_stmt);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Move internal result pointer */
PHP_FUNCTION(mysqli_stmt_data_seek)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;
	zend_long		offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &mysql_stmt, mysqli_stmt_class_entry, &offset) == FAILURE) {
		RETURN_THROWS();
	}

	if (offset < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	mysql_stmt_data_seek(stmt->stmt, offset);
}
/* }}} */

/* {{{ Return the number of result columns for the given statement */
PHP_FUNCTION(mysqli_stmt_field_count)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_stmt_field_count(stmt->stmt));
}
/* }}} */

/* {{{ Free stored result memory for the given statement handle */
PHP_FUNCTION(mysqli_stmt_free_result)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	mysql_stmt_free_result(stmt->stmt);
}
/* }}} */

/* {{{ Get the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysqli_stmt_insert_id)
{
	MY_STMT			*stmt;
	my_ulonglong	rc;
	zval			*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);
	rc = mysql_stmt_insert_id(stmt->stmt);
	MYSQLI_RETURN_LONG_INT(rc)
}
/* }}} */

/* {{{ Return the number of parameter for the given statement */
PHP_FUNCTION(mysqli_stmt_param_count)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_stmt_param_count(stmt->stmt));
}
/* }}} */

/* {{{ reset a prepared statement */
PHP_FUNCTION(mysqli_stmt_reset)
{
	MY_STMT		*stmt;
	zval		*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (mysql_stmt_reset(stmt->stmt)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Return the number of rows in statements result set */
PHP_FUNCTION(mysqli_stmt_num_rows)
{
	MY_STMT			*stmt;
	zval			*mysql_stmt;
	my_ulonglong	rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	rc = mysql_stmt_num_rows(stmt->stmt);
	MYSQLI_RETURN_LONG_INT(rc)
}
/* }}} */

/* {{{ Select a MySQL database */
PHP_FUNCTION(mysqli_select_db)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*dbname;
	size_t			dbname_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &dbname, &dbname_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_select_db(mysql->mysql, dbname)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns the SQLSTATE error from previous MySQL operation */
PHP_FUNCTION(mysqli_sqlstate)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	RETURN_STRING(mysql_sqlstate(mysql->mysql));
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_ssl_set)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*ssl_parm[5];
	size_t			ssl_parm_len[5], i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os!s!s!s!s!", &mysql_link, mysqli_link_class_entry, &ssl_parm[0], &ssl_parm_len[0], &ssl_parm[1], &ssl_parm_len[1], &ssl_parm[2], &ssl_parm_len[2], &ssl_parm[3], &ssl_parm_len[3], &ssl_parm[4], &ssl_parm_len[4])   == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_INITIALIZED);

	for (i = 0; i < 5; i++) {
		if (!ssl_parm_len[i]) {
			ssl_parm[i] = NULL;
		}
	}

	mysql_ssl_set(mysql->mysql, ssl_parm[0], ssl_parm[1], ssl_parm[2], ssl_parm[3], ssl_parm[4]);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Get current system status */
PHP_FUNCTION(mysqli_stat)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_string *stat;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysqlnd_stat(mysql->mysql, &stat) == PASS)
	{
		RETURN_STR(stat);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ Flush tables or caches, or reset replication server information */
PHP_FUNCTION(mysqli_refresh)
{
	MY_MYSQL *mysql;
	zval *mysql_link = NULL;
	zend_long options;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &mysql_link, mysqli_link_class_entry, &options) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_INITIALIZED);
	RETURN_BOOL(!mysql_refresh(mysql->mysql, (uint8_t) options));
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_attr_set)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;
	zend_long	mode_in;
	my_bool mode_b;
	unsigned long	mode;
	zend_long	attr;
	void	*mode_p;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll", &mysql_stmt, mysqli_stmt_class_entry, &attr, &mode_in) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	switch (attr) {
	case STMT_ATTR_UPDATE_MAX_LENGTH:
		if (mode_in != 0 && mode_in != 1) {
			zend_argument_value_error(ERROR_ARG_POS(3), "must be 0 or 1 for attribute MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH");
			RETURN_THROWS();
		}
		mode_b = (my_bool) mode_in;
		mode_p = &mode_b;
		break;
	case STMT_ATTR_CURSOR_TYPE:
		switch (mode_in) {
			case CURSOR_TYPE_NO_CURSOR:
			case CURSOR_TYPE_READ_ONLY:
			case CURSOR_TYPE_FOR_UPDATE:
			case CURSOR_TYPE_SCROLLABLE:
				break;
			default:
				zend_argument_value_error(ERROR_ARG_POS(3), "must be one of the MYSQLI_CURSOR_TYPE_* constants "
					"for attribute MYSQLI_STMT_ATTR_CURSOR_TYPE");
				RETURN_THROWS();
		}
		mode = mode_in;
		mode_p = &mode;
		break;
	case STMT_ATTR_PREFETCH_ROWS:
		if (mode_in < 1) {
			zend_argument_value_error(ERROR_ARG_POS(3), "must be greater than 0 for attribute MYSQLI_STMT_ATTR_PREFETCH_ROWS");
			RETURN_THROWS();
		}
		mode = mode_in;
		mode_p = &mode;
		break;
	default:
		zend_argument_value_error(ERROR_ARG_POS(2), "must be one of "
			"MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, "
			"MYSQLI_STMT_ATTR_PREFETCH_ROWS, or STMT_ATTR_CURSOR_TYPE");
		RETURN_THROWS();
	}

	if (FAIL == mysql_stmt_attr_set(stmt->stmt, attr, mode_p)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_attr_get)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;
	unsigned long	value = 0;
	zend_long	attr;
	int		rc;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &mysql_stmt, mysqli_stmt_class_entry, &attr) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if ((rc = mysql_stmt_attr_get(stmt->stmt, attr, &value))) {
		/* Success corresponds to 0 return value and a non-zero value
		 * should only happen if the attr/option is unknown */
		zend_argument_value_error(ERROR_ARG_POS(2), "must be one of "
			"MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, "
			"MYSQLI_STMT_ATTR_PREFETCH_ROWS, or STMT_ATTR_CURSOR_TYPE");
		RETURN_THROWS();
	}

	if (attr == STMT_ATTR_UPDATE_MAX_LENGTH)
		value = (my_bool)value;

	RETURN_LONG((unsigned long)value);
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_errno)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_INITIALIZED);

	RETURN_LONG(mysql_stmt_errno(stmt->stmt));
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_error)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_INITIALIZED);

	RETURN_STRING(mysql_stmt_error(stmt->stmt));
}
/* }}} */

/* {{{ Initialize statement object */
PHP_FUNCTION(mysqli_stmt_init)
{
	MY_MYSQL		*mysql;
	MY_STMT			*stmt;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",&mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	stmt = (MY_STMT *)ecalloc(1,sizeof(MY_STMT));

	if (!(stmt->stmt = mysql_stmt_init(mysql->mysql))) {
		efree(stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->status = MYSQLI_STATUS_INITIALIZED;
	mysqli_resource->ptr = (void *)stmt;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_stmt_class_entry);
}
/* }}} */

/* {{{ prepare server side statement with query */
PHP_FUNCTION(mysqli_stmt_prepare)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;
	char	*query;
	size_t		query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_stmt, mysqli_stmt_class_entry, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_INITIALIZED);

	if (mysql_stmt_prepare(stmt->stmt, query, query_len)) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	/* change status */
	MYSQLI_SET_STATUS(mysql_stmt, MYSQLI_STATUS_VALID);
	RETURN_TRUE;
}
/* }}} */

/* {{{ return result set from statement */
PHP_FUNCTION(mysqli_stmt_result_metadata)
{
	MY_STMT			*stmt;
	MYSQL_RES		*result;
	zval			*mysql_stmt;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (!(result = mysql_stmt_result_metadata(stmt->stmt))){
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_store_result)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (mysql_stmt_store_result(stmt->stmt)){
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_sqlstate)
{
	MY_STMT	*stmt;
	zval	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	RETURN_STRING(mysql_stmt_sqlstate(stmt->stmt));
}
/* }}} */

/* {{{ Buffer result set on client */
PHP_FUNCTION(mysqli_store_result)
{
	MY_MYSQL		*mysql;
	MYSQL_RES		*result;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;
	zend_long flags = 0;


	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l", &mysql_link, mysqli_link_class_entry, &flags) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	result = mysql_store_result(mysql->mysql);
	if (!result) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index("from previous query", mysqli_server_status(mysql->mysql));
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ Return the current thread ID */
PHP_FUNCTION(mysqli_thread_id)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	RETURN_LONG((zend_long) mysql_thread_id(mysql->mysql));
}
/* }}} */

/* {{{ Return whether thread safety is given or not */
PHP_FUNCTION(mysqli_thread_safe)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(mysql_thread_safe());
}
/* }}} */

/* {{{ Directly retrieve query results - do not buffer results on client side */
PHP_FUNCTION(mysqli_use_result)
{
	MY_MYSQL		*mysql;
	MYSQL_RES		*result;
	zval			*mysql_link;
	MYSQLI_RESOURCE	*mysqli_resource;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (!(result = mysql_use_result(mysql->mysql))) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index("from previous query", mysqli_server_status(mysql->mysql));
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ Return number of warnings from the last query for the given link */
PHP_FUNCTION(mysqli_warning_count)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_warning_count(mysql->mysql));
}
/* }}} */
