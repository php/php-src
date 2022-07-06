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

/* The PDO Database Handle Class */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pdo.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"
#include "zend_exceptions.h"
#include "zend_object_handlers.h"
#include "zend_hash.h"
#include "zend_interfaces.h"
#include "pdo_dbh_arginfo.h"

static zend_result pdo_dbh_attribute_set(pdo_dbh_t *dbh, zend_long attr, zval *value);

void pdo_throw_exception(unsigned int driver_errcode, char *driver_errmsg, pdo_error_type *pdo_error)
{
		zval error_info,pdo_exception;
		char *pdo_exception_message;

		object_init_ex(&pdo_exception, php_pdo_get_exception());
		array_init(&error_info);

		add_next_index_string(&error_info, *pdo_error);
		add_next_index_long(&error_info, driver_errcode);
		add_next_index_string(&error_info, driver_errmsg);

		spprintf(&pdo_exception_message, 0,"SQLSTATE[%s] [%d] %s",*pdo_error, driver_errcode, driver_errmsg);
		zend_update_property(php_pdo_get_exception(), Z_OBJ(pdo_exception), "errorInfo", sizeof("errorInfo")-1, &error_info);
		zend_update_property_long(php_pdo_get_exception(), Z_OBJ(pdo_exception), "code", sizeof("code")-1, driver_errcode);
		zend_update_property_string(
			php_pdo_get_exception(),
			Z_OBJ(pdo_exception),
			"message",
			sizeof("message")-1,
			pdo_exception_message
		);
		efree(pdo_exception_message);
		zval_ptr_dtor(&error_info);
		zend_throw_exception_object(&pdo_exception);
}

void pdo_raise_impl_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *sqlstate, const char *supp) /* {{{ */
{
	pdo_error_type *pdo_err = &dbh->error_code;
	char *message = NULL;
	const char *msg;

	if (dbh && dbh->error_mode == PDO_ERRMODE_SILENT) {
#if 0
		/* BUG: if user is running in silent mode and hits an error at the driver level
		 * when they use the PDO methods to call up the error information, they may
		 * get bogus information */
		return;
#endif
	}

	if (stmt) {
		pdo_err = &stmt->error_code;
	}

	strncpy(*pdo_err, sqlstate, 6);

	/* hash sqlstate to error messages */
	msg = pdo_sqlstate_state_to_description(*pdo_err);
	if (!msg) {
		msg = "<<Unknown error>>";
	}

	if (supp) {
		spprintf(&message, 0, "SQLSTATE[%s]: %s: %s", *pdo_err, msg, supp);
	} else {
		spprintf(&message, 0, "SQLSTATE[%s]: %s", *pdo_err, msg);
	}

	if (dbh && dbh->error_mode != PDO_ERRMODE_EXCEPTION) {
		php_error_docref(NULL, E_WARNING, "%s", message);
	} else {
		zval ex, info;
		zend_class_entry *pdo_ex = php_pdo_get_exception();

		object_init_ex(&ex, pdo_ex);

		zend_update_property_string(zend_ce_exception, Z_OBJ(ex), "message", sizeof("message")-1, message);
		zend_update_property_string(zend_ce_exception, Z_OBJ(ex), "code", sizeof("code")-1, *pdo_err);

		array_init(&info);

		add_next_index_string(&info, *pdo_err);
		add_next_index_long(&info, 0);
		zend_update_property(pdo_ex, Z_OBJ(ex), "errorInfo", sizeof("errorInfo")-1, &info);
		zval_ptr_dtor(&info);

		zend_throw_exception_object(&ex);
	}

	if (message) {
		efree(message);
	}
}
/* }}} */

PDO_API void pdo_handle_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt) /* {{{ */
{
	pdo_error_type *pdo_err = &dbh->error_code;
	const char *msg = "<<Unknown>>";
	char *supp = NULL;
	zend_long native_code = 0;
	zend_string *message = NULL;
	zval info;

	if (dbh == NULL || dbh->error_mode == PDO_ERRMODE_SILENT) {
		return;
	}

	if (stmt) {
		pdo_err = &stmt->error_code;
	}

	/* hash sqlstate to error messages */
	msg = pdo_sqlstate_state_to_description(*pdo_err);
	if (!msg) {
		msg = "<<Unknown error>>";
	}

	ZVAL_UNDEF(&info);
	if (dbh->methods->fetch_err) {
		array_init(&info);

		add_next_index_string(&info, *pdo_err);

		if (dbh->methods->fetch_err(dbh, stmt, &info)) {
			zval *item;

			if ((item = zend_hash_index_find(Z_ARRVAL(info), 1)) != NULL
					&& Z_TYPE_P(item) == IS_LONG) {
				native_code = Z_LVAL_P(item);
			}

			if ((item = zend_hash_index_find(Z_ARRVAL(info), 2)) != NULL) {
				supp = estrndup(Z_STRVAL_P(item), Z_STRLEN_P(item));
			}
		}
	}

	if (native_code && supp) {
		message = strpprintf(0, "SQLSTATE[%s]: %s: " ZEND_LONG_FMT " %s", *pdo_err, msg, native_code, supp);
	} else if (supp) {
		message = strpprintf(0, "SQLSTATE[%s]: %s: %s", *pdo_err, msg, supp);
	} else {
		message = strpprintf(0, "SQLSTATE[%s]: %s", *pdo_err, msg);
	}

	if (dbh->error_mode == PDO_ERRMODE_WARNING) {
		php_error_docref(NULL, E_WARNING, "%s", ZSTR_VAL(message));
	} else if (EG(exception) == NULL) {
		zval ex;
		zend_class_entry *pdo_ex = php_pdo_get_exception();

		object_init_ex(&ex, pdo_ex);

		zend_update_property_str(zend_ce_exception, Z_OBJ(ex), "message", sizeof("message") - 1, message);
		zend_update_property_string(zend_ce_exception, Z_OBJ(ex), "code", sizeof("code") - 1, *pdo_err);

		if (!Z_ISUNDEF(info)) {
			zend_update_property(pdo_ex, Z_OBJ(ex), "errorInfo", sizeof("errorInfo") - 1, &info);
		}

		zend_throw_exception_object(&ex);
	}

	if (!Z_ISUNDEF(info)) {
		zval_ptr_dtor(&info);
	}

	if (message) {
		zend_string_release_ex(message, 0);
	}

	if (supp) {
		efree(supp);
	}
}
/* }}} */

static char *dsn_from_uri(char *uri, char *buf, size_t buflen) /* {{{ */
{
	php_stream *stream;
	char *dsn = NULL;

	stream = php_stream_open_wrapper(uri, "rb", REPORT_ERRORS, NULL);
	if (stream) {
		dsn = php_stream_get_line(stream, buf, buflen, NULL);
		php_stream_close(stream);
	}
	return dsn;
}
/* }}} */

/* {{{ */
PHP_METHOD(PDO, __construct)
{
	zval *object = ZEND_THIS;
	pdo_dbh_t *dbh = NULL;
	zend_bool is_persistent = 0;
	char *data_source;
	size_t data_source_len;
	char *colon;
	char *username=NULL, *password=NULL;
	size_t usernamelen, passwordlen;
	pdo_driver_t *driver = NULL;
	zval *options = NULL;
	char alt_dsn[512];
	int call_factory = 1;
	zend_error_handling zeh;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(data_source, data_source_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(username, usernamelen)
		Z_PARAM_STRING_OR_NULL(password, passwordlen)
		Z_PARAM_ARRAY_OR_NULL(options)
	ZEND_PARSE_PARAMETERS_END();

	/* parse the data source name */
	colon = strchr(data_source, ':');

	if (!colon) {
		/* let's see if this string has a matching dsn in the php.ini */
		char *ini_dsn = NULL;

		snprintf(alt_dsn, sizeof(alt_dsn), "pdo.dsn.%s", data_source);
		if (FAILURE == cfg_get_string(alt_dsn, &ini_dsn)) {
			zend_argument_error(php_pdo_get_exception(), 1, "must be a valid data source name");
			RETURN_THROWS();
		}

		data_source = ini_dsn;
		colon = strchr(data_source, ':');

		if (!colon) {
			zend_throw_exception_ex(php_pdo_get_exception(), 0, "invalid data source name (via INI: %s)", alt_dsn);
			RETURN_THROWS();
		}
	}

	if (!strncmp(data_source, "uri:", sizeof("uri:")-1)) {
		/* the specified URI holds connection details */
		data_source = dsn_from_uri(data_source + sizeof("uri:")-1, alt_dsn, sizeof(alt_dsn));
		if (!data_source) {
			zend_argument_error(php_pdo_get_exception(), 1, "must be a valid data source URI");
			RETURN_THROWS();
		}
		colon = strchr(data_source, ':');
		if (!colon) {
			zend_argument_error(php_pdo_get_exception(), 1, "must be a valid data source name (via URI)");
			RETURN_THROWS();
		}
	}

	driver = pdo_find_driver(data_source, colon - data_source);

	if (!driver) {
		/* NB: don't want to include the data_source in the error message as
		 * it might contain a password */
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "could not find driver");
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(object);

	/* is this supposed to be a persistent connection ? */
	if (options) {
		int plen = 0;
		char *hashkey = NULL;
		zend_resource *le;
		pdo_dbh_t *pdbh = NULL;
		zval *v;

		if ((v = zend_hash_index_find_deref(Z_ARRVAL_P(options), PDO_ATTR_PERSISTENT)) != NULL) {
			if (Z_TYPE_P(v) == IS_STRING &&
				!is_numeric_string(Z_STRVAL_P(v), Z_STRLEN_P(v), NULL, NULL, 0) && Z_STRLEN_P(v) > 0) {
				/* user specified key */
				plen = spprintf(&hashkey, 0, "PDO:DBH:DSN=%s:%s:%s:%s", data_source,
						username ? username : "",
						password ? password : "",
						Z_STRVAL_P(v));
				is_persistent = 1;
			} else {
				is_persistent = zval_get_long(v) ? 1 : 0;
				plen = spprintf(&hashkey, 0, "PDO:DBH:DSN=%s:%s:%s", data_source,
						username ? username : "",
						password ? password : "");
			}
		}

		if (is_persistent) {
			/* let's see if we have one cached.... */
			if ((le = zend_hash_str_find_ptr(&EG(persistent_list), hashkey, plen)) != NULL) {
				if (le->type == php_pdo_list_entry()) {
					pdbh = (pdo_dbh_t*)le->ptr;

					/* is the connection still alive ? */
					if (pdbh->methods->check_liveness && FAILURE == (pdbh->methods->check_liveness)(pdbh)) {
						/* nope... need to kill it */
						pdbh->refcount--;
						zend_list_close(le);
						pdbh = NULL;
					}
				}
			}

			if (pdbh) {
				call_factory = 0;
			} else {
				/* need a brand new pdbh */
				pdbh = pecalloc(1, sizeof(*pdbh), 1);

				pdbh->refcount = 1;
				pdbh->is_persistent = 1;
				pdbh->persistent_id = pemalloc(plen + 1, 1);
				memcpy((char *)pdbh->persistent_id, hashkey, plen+1);
				pdbh->persistent_id_len = plen;
				pdbh->def_stmt_ce = dbh->def_stmt_ce;
			}
		}

		if (pdbh) {
			efree(dbh);
			/* switch over to the persistent one */
			Z_PDO_OBJECT_P(object)->inner = pdbh;
			pdbh->refcount++;
			dbh = pdbh;
		}

		if (hashkey) {
			efree(hashkey);
		}
	}

	if (call_factory) {
		dbh->data_source_len = strlen(colon + 1);
		dbh->data_source = (const char*)pestrdup(colon + 1, is_persistent);
		dbh->username = username ? pestrdup(username, is_persistent) : NULL;
		dbh->password = password ? pestrdup(password, is_persistent) : NULL;
		dbh->default_fetch_type = PDO_FETCH_BOTH;
	}

	dbh->auto_commit = pdo_attr_lval(options, PDO_ATTR_AUTOCOMMIT, 1);
	dbh->error_mode = pdo_attr_lval(options, PDO_ATTR_ERRMODE, PDO_ERRMODE_EXCEPTION);

	if (!dbh->data_source || (username && !dbh->username) || (password && !dbh->password)) {
		php_error_docref(NULL, E_ERROR, "Out of memory");
	}

	zend_replace_error_handling(EH_THROW, pdo_exception_ce, &zeh);

	if (!call_factory) {
		/* we got a persistent guy from our cache */
		goto options;
	}

	if (driver->db_handle_factory(dbh, options)) {
		/* all set */

		if (is_persistent) {
			/* register in the persistent list etc. */
			/* we should also need to replace the object store entry,
			   since it was created with emalloc */
			if ((zend_register_persistent_resource(
						(char*)dbh->persistent_id, dbh->persistent_id_len, dbh, php_pdo_list_entry())) == NULL) {
				php_error_docref(NULL, E_ERROR, "Failed to register persistent entry");
			}
		}

		dbh->driver = driver;
options:
		if (options) {
			zval *attr_value;
			zend_ulong long_key;
			zend_string *str_key = NULL;

			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(options), long_key, str_key, attr_value) {
				if (str_key) {
					continue;
				}
				ZVAL_DEREF(attr_value);
				/* TODO: Check that this doesn't fail? */
				pdo_dbh_attribute_set(dbh, long_key, attr_value);
			} ZEND_HASH_FOREACH_END();
		}

		zend_restore_error_handling(&zeh);
		return;
	}

	/* the connection failed; things will tidy up in free_storage */
	/* XXX raise exception */
	zend_restore_error_handling(&zeh);
	if (!EG(exception)) {
		zend_throw_exception(pdo_exception_ce, "Constructor failed", 0);
	}
}
/* }}} */

static zval *pdo_stmt_instantiate(pdo_dbh_t *dbh, zval *object, zend_class_entry *dbstmt_ce, zval *ctor_args) /* {{{ */
{
	if (!Z_ISUNDEF_P(ctor_args)) {
		/* This implies an error within PDO if this does not hold */
		ZEND_ASSERT(Z_TYPE_P(ctor_args) == IS_ARRAY);
		if (!dbstmt_ce->constructor) {
			zend_throw_error(NULL, "User-supplied statement does not accept constructor arguments");
			return NULL;
		}
	}

	if (UNEXPECTED(object_init_ex(object, dbstmt_ce) != SUCCESS)) {
		if (EXPECTED(!EG(exception))) {
			zend_throw_error(NULL, "Cannot instantiate user-supplied statement class");
		}
		return NULL;
	}

	return object;
} /* }}} */

static void pdo_stmt_construct(zend_execute_data *execute_data, pdo_stmt_t *stmt, zval *object, zend_class_entry *dbstmt_ce, zval *ctor_args) /* {{{ */
{
	zval query_string;
	zend_string *key;

	ZVAL_STRINGL(&query_string, stmt->query_string, stmt->query_stringlen);
	key = zend_string_init("queryString", sizeof("queryString") - 1, 0);
	zend_std_write_property(Z_OBJ_P(object), key, &query_string, NULL);
	zval_ptr_dtor(&query_string);
	zend_string_release_ex(key, 0);

	if (dbstmt_ce->constructor) {
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval retval;

		fci.size = sizeof(zend_fcall_info);
		ZVAL_UNDEF(&fci.function_name);
		fci.object = Z_OBJ_P(object);
		fci.retval = &retval;
		fci.param_count = 0;
		fci.params = NULL;
		fci.named_params = NULL;

		zend_fcall_info_args(&fci, ctor_args);

		fcc.function_handler = dbstmt_ce->constructor;
		fcc.called_scope = Z_OBJCE_P(object);
		fcc.object = Z_OBJ_P(object);

		if (zend_call_function(&fci, &fcc) != FAILURE) {
			zval_ptr_dtor(&retval);
		}

		zend_fcall_info_args_clear(&fci, 1);
	}
}
/* }}} */

/* {{{ Prepares a statement for execution and returns a statement object */
PHP_METHOD(PDO, prepare)
{
	pdo_stmt_t *stmt;
	char *statement;
	size_t statement_len;
	zval *options = NULL, *value, *item, ctor_args;
	zend_class_entry *dbstmt_ce, *pce;
	pdo_dbh_object_t *dbh_obj = Z_PDO_OBJECT_P(ZEND_THIS);
	pdo_dbh_t *dbh = dbh_obj->inner;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(statement, statement_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	PDO_CONSTRUCT_CHECK;

	if (statement_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	PDO_DBH_CLEAR_ERR();

	if (options && (value = zend_hash_index_find(Z_ARRVAL_P(options), PDO_ATTR_STATEMENT_CLASS)) != NULL) {
		if (Z_TYPE_P(value) != IS_ARRAY) {
			zend_type_error("PDO::ATTR_STATEMENT_CLASS value must be of type array, %s given",
				zend_zval_type_name(value));
			RETURN_THROWS();
		}
		if ((item = zend_hash_index_find(Z_ARRVAL_P(value), 0)) == NULL) {
			zend_value_error("PDO::ATTR_STATEMENT_CLASS value must be an array with the format "
				"array(classname, constructor_args)");
			RETURN_THROWS();
		}
		if (Z_TYPE_P(item) != IS_STRING || (pce = zend_lookup_class(Z_STR_P(item))) == NULL) {
			zend_type_error("PDO::ATTR_STATEMENT_CLASS class must be a valid class");
			RETURN_THROWS();
		}
		dbstmt_ce = pce;
		if (!instanceof_function(dbstmt_ce, pdo_dbstmt_ce)) {
			zend_type_error("PDO::ATTR_STATEMENT_CLASS class must be derived from PDOStatement");
			RETURN_THROWS();
		}
		if (dbstmt_ce->constructor && !(dbstmt_ce->constructor->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED))) {
			zend_type_error("User-supplied statement class cannot have a public constructor");
			RETURN_THROWS();
		}
		if ((item = zend_hash_index_find(Z_ARRVAL_P(value), 1)) != NULL) {
			if (Z_TYPE_P(item) != IS_ARRAY) {
				zend_type_error("PDO::ATTR_STATEMENT_CLASS constructor_args must be of type ?array, %s given",
					zend_zval_type_name(value));
				RETURN_THROWS();
			}
			ZVAL_COPY_VALUE(&ctor_args, item);
		} else {
			ZVAL_UNDEF(&ctor_args);
		}
	} else {
		dbstmt_ce = dbh->def_stmt_ce;
		ZVAL_COPY_VALUE(&ctor_args, &dbh->def_stmt_ctor_args);
	}

	if (!pdo_stmt_instantiate(dbh, return_value, dbstmt_ce, &ctor_args)) {
		RETURN_THROWS();
	}
	stmt = Z_PDO_STMT_P(return_value);

	/* unconditionally keep this for later reference */
	stmt->query_string = estrndup(statement, statement_len);
	stmt->query_stringlen = statement_len;
	stmt->default_fetch_type = dbh->default_fetch_type;
	stmt->dbh = dbh;
	/* give it a reference to me */
	ZVAL_OBJ_COPY(&stmt->database_object_handle, &dbh_obj->std);
	/* we haven't created a lazy object yet */
	ZVAL_UNDEF(&stmt->lazy_object_ref);

	if (dbh->methods->preparer(dbh, statement, statement_len, stmt, options)) {
		pdo_stmt_construct(execute_data, stmt, return_value, dbstmt_ce, &ctor_args);
		return;
	}

	PDO_HANDLE_DBH_ERR();

	/* kill the object handle for the stmt here */
	zval_ptr_dtor(return_value);

	RETURN_FALSE;
}
/* }}} */


static zend_bool pdo_is_in_transaction(pdo_dbh_t *dbh) {
	if (dbh->methods->in_transaction) {
		return dbh->methods->in_transaction(dbh);
	}
	return dbh->in_txn;
}

/* {{{ Initiates a transaction */
PHP_METHOD(PDO, beginTransaction)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	PDO_CONSTRUCT_CHECK;

	if (pdo_is_in_transaction(dbh)) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "There is already an active transaction");
		RETURN_THROWS();
	}

	if (!dbh->methods->begin) {
		/* TODO: this should be an exception; see the auto-commit mode
		 * comments below */
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "This driver doesn't support transactions");
		RETURN_THROWS();
	}

	if (dbh->methods->begin(dbh)) {
		dbh->in_txn = 1;
		RETURN_TRUE;
	}

	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ Commit a transaction */
PHP_METHOD(PDO, commit)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	PDO_CONSTRUCT_CHECK;

	if (!pdo_is_in_transaction(dbh)) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "There is no active transaction");
		RETURN_THROWS();
	}

	if (dbh->methods->commit(dbh)) {
		dbh->in_txn = 0;
		RETURN_TRUE;
	}

	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ roll back a transaction */
PHP_METHOD(PDO, rollBack)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	PDO_CONSTRUCT_CHECK;

	if (!pdo_is_in_transaction(dbh)) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "There is no active transaction");
		RETURN_THROWS();
	}

	if (dbh->methods->rollback(dbh)) {
		dbh->in_txn = 0;
		RETURN_TRUE;
	}

	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ determine if inside a transaction */
PHP_METHOD(PDO, inTransaction)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	PDO_CONSTRUCT_CHECK;

	RETURN_BOOL(pdo_is_in_transaction(dbh));
}
/* }}} */

static zend_result pdo_dbh_attribute_set(pdo_dbh_t *dbh, zend_long attr, zval *value) /* {{{ */
{
	zend_long lval;

/* TODO: Make distinction between numeric and non-numeric strings */
#define PDO_LONG_PARAM_CHECK \
	if (Z_TYPE_P(value) != IS_LONG && Z_TYPE_P(value) != IS_STRING && Z_TYPE_P(value) != IS_FALSE && Z_TYPE_P(value) != IS_TRUE) { \
		zend_type_error("Attribute value must be of type int for selected attribute, %s given", zend_zval_type_name(value)); \
		return FAILURE; \
	} \

	switch (attr) {
		case PDO_ATTR_ERRMODE:
			PDO_LONG_PARAM_CHECK;
			lval = zval_get_long(value);
			switch (lval) {
				case PDO_ERRMODE_SILENT:
				case PDO_ERRMODE_WARNING:
				case PDO_ERRMODE_EXCEPTION:
					dbh->error_mode = lval;
					return SUCCESS;
				default:
					zend_value_error("Error mode must be one of the PDO::ERRMODE_* constants");
					return FAILURE;
			}
			return FAILURE;

		case PDO_ATTR_CASE:
			PDO_LONG_PARAM_CHECK;
			lval = zval_get_long(value);
			switch (lval) {
				case PDO_CASE_NATURAL:
				case PDO_CASE_UPPER:
				case PDO_CASE_LOWER:
					dbh->desired_case = lval;
					return SUCCESS;
				default:
					zend_value_error("Case folding mode must be one of the PDO::CASE_* constants");
					return FAILURE;
			}
			return FAILURE;

		case PDO_ATTR_ORACLE_NULLS:
			PDO_LONG_PARAM_CHECK;
			dbh->oracle_nulls = zval_get_long(value);
			return SUCCESS;

		case PDO_ATTR_DEFAULT_FETCH_MODE:
			if (Z_TYPE_P(value) == IS_ARRAY) {
				zval *tmp;
				if ((tmp = zend_hash_index_find(Z_ARRVAL_P(value), 0)) != NULL && Z_TYPE_P(tmp) == IS_LONG) {
					if (Z_LVAL_P(tmp) == PDO_FETCH_INTO || Z_LVAL_P(tmp) == PDO_FETCH_CLASS) {
						zend_value_error("PDO::FETCH_INTO and PDO::FETCH_CLASS cannot be set as the default fetch mode");
						return FAILURE;
					}
				}
			} else {
				PDO_LONG_PARAM_CHECK;
			}
			lval = zval_get_long(value);
			if (lval == PDO_FETCH_USE_DEFAULT) {
				zend_value_error("Fetch mode must be a bitmask of PDO::FETCH_* constants");
				return FAILURE;
			}
			dbh->default_fetch_type = lval;
			return SUCCESS;

		case PDO_ATTR_STRINGIFY_FETCHES:
			PDO_LONG_PARAM_CHECK;
			dbh->stringify = zval_get_long(value) ? 1 : 0;
			return SUCCESS;

		case PDO_ATTR_STATEMENT_CLASS: {
			/* array(string classname, array(mixed ctor_args)) */
			zend_class_entry *pce;
			zval *item;

			if (dbh->is_persistent) {
				/* TODO: ValueError/ PDOException? */
				pdo_raise_impl_error(dbh, NULL, "HY000",
					"PDO::ATTR_STATEMENT_CLASS cannot be used with persistent PDO instances"
					);
				PDO_HANDLE_DBH_ERR();
				return FAILURE;
			}
			if (Z_TYPE_P(value) != IS_ARRAY) {
				zend_type_error("PDO::ATTR_STATEMENT_CLASS value must be of type array, %s given",
					zend_zval_type_name(value));
				return FAILURE;
			}
			if ((item = zend_hash_index_find(Z_ARRVAL_P(value), 0)) == NULL) {
				zend_value_error("PDO::ATTR_STATEMENT_CLASS value must be an array with the format "
					"array(classname, constructor_args)");
				return FAILURE;
			}
			if (Z_TYPE_P(item) != IS_STRING || (pce = zend_lookup_class(Z_STR_P(item))) == NULL) {
				zend_type_error("PDO::ATTR_STATEMENT_CLASS class must be a valid class");
				return FAILURE;
			}
			if (!instanceof_function(pce, pdo_dbstmt_ce)) {
				zend_type_error("PDO::ATTR_STATEMENT_CLASS class must be derived from PDOStatement");
				return FAILURE;
			}
			if (pce->constructor && !(pce->constructor->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED))) {
				zend_type_error("User-supplied statement class cannot have a public constructor");
				return FAILURE;
			}
			dbh->def_stmt_ce = pce;
			if (!Z_ISUNDEF(dbh->def_stmt_ctor_args)) {
				zval_ptr_dtor(&dbh->def_stmt_ctor_args);
				ZVAL_UNDEF(&dbh->def_stmt_ctor_args);
			}
			if ((item = zend_hash_index_find(Z_ARRVAL_P(value), 1)) != NULL) {
				if (Z_TYPE_P(item) != IS_ARRAY) {
					zend_type_error("PDO::ATTR_STATEMENT_CLASS constructor_args must be of type ?array, %s given",
						zend_zval_type_name(value));
					return FAILURE;
				}
				ZVAL_COPY(&dbh->def_stmt_ctor_args, item);
			}
			return SUCCESS;
		}

		default:
			;
	}

	if (!dbh->methods->set_attribute) {
		goto fail;
	}

	PDO_DBH_CLEAR_ERR();
	if (dbh->methods->set_attribute(dbh, attr, value)) {
		return SUCCESS;
	}

fail:
	if (!dbh->methods->set_attribute) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support setting attributes");
	} else {
		PDO_HANDLE_DBH_ERR();
	}
	return FAILURE;
}
/* }}} */

/* {{{ Set an attribute */
PHP_METHOD(PDO, setAttribute)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);
	zend_long attr;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(attr)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	PDO_DBH_CLEAR_ERR();
	PDO_CONSTRUCT_CHECK;

	if (pdo_dbh_attribute_set(dbh, attr, value) != FAILURE) {
 		RETURN_TRUE;
 	}
 	RETURN_FALSE;
}
/* }}} */

/* {{{ Get an attribute */
PHP_METHOD(PDO, getAttribute)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);
	zend_long attr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(attr)
	ZEND_PARSE_PARAMETERS_END();

	PDO_DBH_CLEAR_ERR();
	PDO_CONSTRUCT_CHECK;

	/* handle generic PDO-level attributes */
	switch (attr) {
		case PDO_ATTR_PERSISTENT:
			RETURN_BOOL(dbh->is_persistent);

		case PDO_ATTR_CASE:
			RETURN_LONG(dbh->desired_case);

		case PDO_ATTR_ORACLE_NULLS:
			RETURN_LONG(dbh->oracle_nulls);

		case PDO_ATTR_ERRMODE:
			RETURN_LONG(dbh->error_mode);

		case PDO_ATTR_DRIVER_NAME:
			RETURN_STRINGL((char*)dbh->driver->driver_name, dbh->driver->driver_name_len);

		case PDO_ATTR_STATEMENT_CLASS:
			array_init(return_value);
			add_next_index_str(return_value, zend_string_copy(dbh->def_stmt_ce->name));
			if (!Z_ISUNDEF(dbh->def_stmt_ctor_args)) {
				Z_TRY_ADDREF(dbh->def_stmt_ctor_args);
				add_next_index_zval(return_value, &dbh->def_stmt_ctor_args);
			}
			return;
		case PDO_ATTR_DEFAULT_FETCH_MODE:
			RETURN_LONG(dbh->default_fetch_type);
		default:
			break;
	}

	if (!dbh->methods->get_attribute) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support getting attributes");
		RETURN_FALSE;
	}

	switch (dbh->methods->get_attribute(dbh, attr, return_value)) {
		case -1:
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;

		case 0:
			pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support that attribute");
			RETURN_FALSE;

		default:
			return;
	}
}
/* }}} */

/* {{{ Execute a statement that does not return a row set, returning the number of affected rows */
PHP_METHOD(PDO, exec)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);
	char *statement;
	size_t statement_len;
	zend_long ret;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(statement, statement_len)
	ZEND_PARSE_PARAMETERS_END();

	if (statement_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	PDO_DBH_CLEAR_ERR();
	PDO_CONSTRUCT_CHECK;
	ret = dbh->methods->doer(dbh, statement, statement_len);
	if(ret == -1) {
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ Returns the id of the last row that we affected on this connection. Some databases require a sequence or table name to be passed in. Not always meaningful. */
PHP_METHOD(PDO, lastInsertId)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);
	char *name = NULL;
	size_t namelen;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(name, namelen)
	ZEND_PARSE_PARAMETERS_END();

	PDO_CONSTRUCT_CHECK;

	PDO_DBH_CLEAR_ERR();

	if (!dbh->methods->last_id) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support lastInsertId()");
		RETURN_FALSE;
	} else {
		size_t id_len;
		char *id;
		id = dbh->methods->last_id(dbh, name, &id_len);
		if (!id) {
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		} else {
			//??? use zend_string ?
			RETVAL_STRINGL(id, id_len);
			efree(id);
		}
	}
}
/* }}} */

/* {{{ Fetch the error code associated with the last operation on the database handle */
PHP_METHOD(PDO, errorCode)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	PDO_CONSTRUCT_CHECK;

	if (dbh->query_stmt) {
		RETURN_STRING(dbh->query_stmt->error_code);
	}

	if (dbh->error_code[0] == '\0') {
		RETURN_NULL();
	}

	/**
	 * Making sure that we fallback to the default implementation
	 * if the dbh->error_code is not null.
	 */
	RETURN_STRING(dbh->error_code);
}
/* }}} */

/* {{{ Fetch extended error information associated with the last operation on the database handle */
PHP_METHOD(PDO, errorInfo)
{
	int error_count;
	int error_count_diff 	 = 0;
	int error_expected_count = 3;

	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	PDO_CONSTRUCT_CHECK;

	array_init(return_value);

	if (dbh->query_stmt) {
		add_next_index_string(return_value, dbh->query_stmt->error_code);
		if(!strncmp(dbh->query_stmt->error_code, PDO_ERR_NONE, sizeof(PDO_ERR_NONE))) goto fill_array;
	} else {
		add_next_index_string(return_value, dbh->error_code);
		if(!strncmp(dbh->error_code, PDO_ERR_NONE, sizeof(PDO_ERR_NONE))) goto fill_array;
	}

	if (dbh->methods->fetch_err) {
		dbh->methods->fetch_err(dbh, dbh->query_stmt, return_value);
	}

fill_array:
	/**
	 * In order to be consistent, we have to make sure we add the good amount
	 * of nulls depending on the current number of elements. We make a simple
	 * difference and add the needed elements
	 */
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

/* {{{ Prepare and execute $sql; returns the statement object for iteration */
PHP_METHOD(PDO, query)
{
	pdo_stmt_t *stmt;
	char *statement;
	size_t statement_len;
	zend_long fetch_mode;
	zend_bool fetch_mode_is_null = 1;
	zval *args = NULL;
	uint32_t num_args = 0;
	pdo_dbh_object_t *dbh_obj = Z_PDO_OBJECT_P(ZEND_THIS);
	pdo_dbh_t *dbh = dbh_obj->inner;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|l!*", &statement, &statement_len,
			&fetch_mode, &fetch_mode_is_null, &args, &num_args)) {
		RETURN_THROWS();
	}

	PDO_CONSTRUCT_CHECK;

	if (statement_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	PDO_DBH_CLEAR_ERR();

	if (!pdo_stmt_instantiate(dbh, return_value, dbh->def_stmt_ce, &dbh->def_stmt_ctor_args)) {
		RETURN_THROWS();
	}
	stmt = Z_PDO_STMT_P(return_value);

	/* unconditionally keep this for later reference */
	stmt->query_string = estrndup(statement, statement_len);
	stmt->query_stringlen = statement_len;

	stmt->default_fetch_type = dbh->default_fetch_type;
	stmt->active_query_string = stmt->query_string;
	stmt->active_query_stringlen = statement_len;
	stmt->dbh = dbh;
	/* give it a reference to me */
	ZVAL_OBJ_COPY(&stmt->database_object_handle, &dbh_obj->std);
	/* we haven't created a lazy object yet */
	ZVAL_UNDEF(&stmt->lazy_object_ref);

	if (dbh->methods->preparer(dbh, statement, statement_len, stmt, NULL)) {
		PDO_STMT_CLEAR_ERR();
		if (fetch_mode_is_null || pdo_stmt_setup_fetch_mode(stmt, fetch_mode, 2, args, num_args)) {
			/* now execute the statement */
			PDO_STMT_CLEAR_ERR();
			if (stmt->methods->executer(stmt)) {
				int ret = 1;
				if (!stmt->executed) {
					if (stmt->dbh->alloc_own_columns) {
						ret = pdo_stmt_describe_columns(stmt);
					}
					stmt->executed = 1;
				}
				if (ret) {
					pdo_stmt_construct(execute_data, stmt, return_value, dbh->def_stmt_ce, &dbh->def_stmt_ctor_args);
					return;
				}
			}
		}
		/* something broke */
		dbh->query_stmt = stmt;
		ZVAL_OBJ(&dbh->query_stmt_zval, Z_OBJ_P(return_value));
		Z_DELREF(stmt->database_object_handle);
		ZVAL_UNDEF(&stmt->database_object_handle);
		PDO_HANDLE_STMT_ERR();
	} else {
		PDO_HANDLE_DBH_ERR();
		zval_ptr_dtor(return_value);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ quotes string for use in a query. The optional paramtype acts as a hint for drivers that have alternate quoting styles. The default value is PDO_PARAM_STR */
PHP_METHOD(PDO, quote)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);
	char *str;
	size_t str_len;
	zend_long paramtype = PDO_PARAM_STR;
	char *qstr;
	size_t qlen;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(paramtype)
	ZEND_PARSE_PARAMETERS_END();

	PDO_CONSTRUCT_CHECK;

	PDO_DBH_CLEAR_ERR();
	if (!dbh->methods->quoter) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support quoting");
		RETURN_FALSE;
	}

	if (dbh->methods->quoter(dbh, str, str_len, &qstr, &qlen, paramtype)) {
		RETVAL_STRINGL(qstr, qlen);
		efree(qstr);
		return;
	}
	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ Return array of available PDO drivers */
PHP_METHOD(PDO, getAvailableDrivers)
{
	pdo_driver_t *pdriver;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	ZEND_HASH_FOREACH_PTR(&pdo_driver_hash, pdriver) {
		add_next_index_stringl(return_value, (char*)pdriver->driver_name, pdriver->driver_name_len);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static void cls_method_dtor(zval *el) /* {{{ */ {
	zend_function *func = (zend_function*)Z_PTR_P(el);
	if (func->common.function_name) {
		zend_string_release_ex(func->common.function_name, 0);
	}
	efree(func);
}
/* }}} */

static void cls_method_pdtor(zval *el) /* {{{ */ {
	zend_function *func = (zend_function*)Z_PTR_P(el);
	if (func->common.function_name) {
		zend_string_release_ex(func->common.function_name, 1);
	}
	pefree(func, 1);
}
/* }}} */

/* {{{ overloaded object handlers for PDO class */
int pdo_hash_methods(pdo_dbh_object_t *dbh_obj, int kind)
{
	const zend_function_entry *funcs;
	zend_internal_function func;
	size_t namelen;
	char *lc_name;
	pdo_dbh_t *dbh = dbh_obj->inner;

	if (!dbh || !dbh->methods || !dbh->methods->get_driver_methods) {
		return 0;
	}
	funcs =	dbh->methods->get_driver_methods(dbh, kind);
	if (!funcs) {
		return 0;
	}

	dbh->cls_methods[kind] = pemalloc(sizeof(HashTable), dbh->is_persistent);
	zend_hash_init(dbh->cls_methods[kind], 8, NULL,
			dbh->is_persistent? cls_method_pdtor : cls_method_dtor, dbh->is_persistent);

	memset(&func, 0, sizeof(func));

	while (funcs->fname) {
		func.type = ZEND_INTERNAL_FUNCTION;
		func.handler = funcs->handler;
		func.function_name = zend_string_init(funcs->fname, strlen(funcs->fname), dbh->is_persistent);
		func.scope = dbh_obj->std.ce;
		func.prototype = NULL;
		if (funcs->flags) {
			func.fn_flags = funcs->flags | ZEND_ACC_NEVER_CACHE;
		} else {
			func.fn_flags = ZEND_ACC_PUBLIC | ZEND_ACC_NEVER_CACHE;
		}
		if (funcs->arg_info) {
			zend_internal_function_info *info = (zend_internal_function_info*)funcs->arg_info;

			func.arg_info = (zend_internal_arg_info*)funcs->arg_info + 1;
			func.num_args = funcs->num_args;
			if (info->required_num_args == (uint32_t)-1) {
				func.required_num_args = funcs->num_args;
			} else {
				func.required_num_args = info->required_num_args;
			}
			if (ZEND_ARG_SEND_MODE(info)) {
				func.fn_flags |= ZEND_ACC_RETURN_REFERENCE;
			}
			if (ZEND_ARG_IS_VARIADIC(&funcs->arg_info[funcs->num_args])) {
				func.fn_flags |= ZEND_ACC_VARIADIC;
				/* Don't count the variadic argument */
				func.num_args--;
			}
		} else {
			func.arg_info = NULL;
			func.num_args = 0;
			func.required_num_args = 0;
		}
		zend_set_function_arg_flags((zend_function*)&func);
		namelen = strlen(funcs->fname);
		lc_name = emalloc(namelen+1);
		zend_str_tolower_copy(lc_name, funcs->fname, namelen);
		zend_hash_str_add_mem(dbh->cls_methods[kind], lc_name, namelen, &func, sizeof(func));
		efree(lc_name);
		funcs++;
	}

	return 1;
}

static zend_function *dbh_method_get(zend_object **object, zend_string *method_name, const zval *key)
{
	zend_function *fbc = NULL;
	pdo_dbh_object_t *dbh_obj = php_pdo_dbh_fetch_object(*object);
	zend_string *lc_method_name;

	if ((fbc = zend_std_get_method(object, method_name, key)) == NULL) {
		/* not a pre-defined method, nor a user-defined method; check
		 * the driver specific methods */
		if (!dbh_obj->inner->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH]) {
			if (!pdo_hash_methods(dbh_obj,
				PDO_DBH_DRIVER_METHOD_KIND_DBH)
				|| !dbh_obj->inner->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH]) {
				goto out;
			}
		}

		lc_method_name = zend_string_tolower(method_name);
		fbc = zend_hash_find_ptr(dbh_obj->inner->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH], lc_method_name);
		zend_string_release_ex(lc_method_name, 0);
	}

out:
	return fbc;
}

static int dbh_compare(zval *object1, zval *object2)
{
	return ZEND_UNCOMPARABLE;
}

static HashTable *dbh_get_gc(zend_object *object, zval **gc_data, int *gc_count)
{
	pdo_dbh_t *dbh = php_pdo_dbh_fetch_inner(object);
	*gc_data = &dbh->def_stmt_ctor_args;
	*gc_count = 1;
	return zend_std_get_properties(object);
}

static zend_object_handlers pdo_dbh_object_handlers;
static void pdo_dbh_free_storage(zend_object *std);

void pdo_dbh_init(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PDO", class_PDO_methods);
	pdo_dbh_ce = zend_register_internal_class(&ce);
	pdo_dbh_ce->create_object = pdo_dbh_new;
	pdo_dbh_ce->serialize = zend_class_serialize_deny;
	pdo_dbh_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&pdo_dbh_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_dbh_object_handlers.offset = XtOffsetOf(pdo_dbh_object_t, std);
	pdo_dbh_object_handlers.dtor_obj = zend_objects_destroy_object;
	pdo_dbh_object_handlers.free_obj = pdo_dbh_free_storage;
	pdo_dbh_object_handlers.clone_obj = NULL;
	pdo_dbh_object_handlers.get_method = dbh_method_get;
	pdo_dbh_object_handlers.compare = dbh_compare;
	pdo_dbh_object_handlers.get_gc = dbh_get_gc;

	REGISTER_PDO_CLASS_CONST_LONG("PARAM_BOOL", (zend_long)PDO_PARAM_BOOL);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_NULL", (zend_long)PDO_PARAM_NULL);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_INT",  (zend_long)PDO_PARAM_INT);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_STR",  (zend_long)PDO_PARAM_STR);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_LOB",  (zend_long)PDO_PARAM_LOB);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_STMT", (zend_long)PDO_PARAM_STMT);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_INPUT_OUTPUT", (zend_long)PDO_PARAM_INPUT_OUTPUT);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_STR_NATL", (zend_long)PDO_PARAM_STR_NATL);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_STR_CHAR", (zend_long)PDO_PARAM_STR_CHAR);


	REGISTER_PDO_CLASS_CONST_LONG("PARAM_EVT_ALLOC",		(zend_long)PDO_PARAM_EVT_ALLOC);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_EVT_FREE",			(zend_long)PDO_PARAM_EVT_FREE);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_EVT_EXEC_PRE",		(zend_long)PDO_PARAM_EVT_EXEC_PRE);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_EVT_EXEC_POST",	(zend_long)PDO_PARAM_EVT_EXEC_POST);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_EVT_FETCH_PRE",	(zend_long)PDO_PARAM_EVT_FETCH_PRE);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_EVT_FETCH_POST",	(zend_long)PDO_PARAM_EVT_FETCH_POST);
	REGISTER_PDO_CLASS_CONST_LONG("PARAM_EVT_NORMALIZE",	(zend_long)PDO_PARAM_EVT_NORMALIZE);

	REGISTER_PDO_CLASS_CONST_LONG("FETCH_DEFAULT", (zend_long)PDO_FETCH_USE_DEFAULT);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_LAZY", (zend_long)PDO_FETCH_LAZY);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_ASSOC", (zend_long)PDO_FETCH_ASSOC);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_NUM",  (zend_long)PDO_FETCH_NUM);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_BOTH", (zend_long)PDO_FETCH_BOTH);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_OBJ",  (zend_long)PDO_FETCH_OBJ);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_BOUND", (zend_long)PDO_FETCH_BOUND);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_COLUMN", (zend_long)PDO_FETCH_COLUMN);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_CLASS", (zend_long)PDO_FETCH_CLASS);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_INTO", (zend_long)PDO_FETCH_INTO);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_FUNC", (zend_long)PDO_FETCH_FUNC);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_GROUP", (zend_long)PDO_FETCH_GROUP);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_UNIQUE", (zend_long)PDO_FETCH_UNIQUE);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_KEY_PAIR", (zend_long)PDO_FETCH_KEY_PAIR);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_CLASSTYPE", (zend_long)PDO_FETCH_CLASSTYPE);

	REGISTER_PDO_CLASS_CONST_LONG("FETCH_SERIALIZE",(zend_long)PDO_FETCH_SERIALIZE);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_PROPS_LATE", (zend_long)PDO_FETCH_PROPS_LATE);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_NAMED", (zend_long)PDO_FETCH_NAMED);

	REGISTER_PDO_CLASS_CONST_LONG("ATTR_AUTOCOMMIT",	(zend_long)PDO_ATTR_AUTOCOMMIT);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_PREFETCH",		(zend_long)PDO_ATTR_PREFETCH);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_TIMEOUT", 		(zend_long)PDO_ATTR_TIMEOUT);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_ERRMODE", 		(zend_long)PDO_ATTR_ERRMODE);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_SERVER_VERSION",	(zend_long)PDO_ATTR_SERVER_VERSION);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_CLIENT_VERSION", 	(zend_long)PDO_ATTR_CLIENT_VERSION);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_SERVER_INFO",		(zend_long)PDO_ATTR_SERVER_INFO);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_CONNECTION_STATUS", 	(zend_long)PDO_ATTR_CONNECTION_STATUS);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_CASE",		 	(zend_long)PDO_ATTR_CASE);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_CURSOR_NAME", 	(zend_long)PDO_ATTR_CURSOR_NAME);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_CURSOR",	 	(zend_long)PDO_ATTR_CURSOR);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_ORACLE_NULLS",	(zend_long)PDO_ATTR_ORACLE_NULLS);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_PERSISTENT",	(zend_long)PDO_ATTR_PERSISTENT);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_STATEMENT_CLASS",		(zend_long)PDO_ATTR_STATEMENT_CLASS);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_FETCH_TABLE_NAMES",		(zend_long)PDO_ATTR_FETCH_TABLE_NAMES);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_FETCH_CATALOG_NAMES",		(zend_long)PDO_ATTR_FETCH_CATALOG_NAMES);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_DRIVER_NAME",		(zend_long)PDO_ATTR_DRIVER_NAME);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_STRINGIFY_FETCHES", (zend_long)PDO_ATTR_STRINGIFY_FETCHES);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_MAX_COLUMN_LEN", (zend_long)PDO_ATTR_MAX_COLUMN_LEN);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_EMULATE_PREPARES", (zend_long)PDO_ATTR_EMULATE_PREPARES);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_DEFAULT_FETCH_MODE", (zend_long)PDO_ATTR_DEFAULT_FETCH_MODE);
	REGISTER_PDO_CLASS_CONST_LONG("ATTR_DEFAULT_STR_PARAM", (zend_long)PDO_ATTR_DEFAULT_STR_PARAM);

	REGISTER_PDO_CLASS_CONST_LONG("ERRMODE_SILENT",	(zend_long)PDO_ERRMODE_SILENT);
	REGISTER_PDO_CLASS_CONST_LONG("ERRMODE_WARNING",	(zend_long)PDO_ERRMODE_WARNING);
	REGISTER_PDO_CLASS_CONST_LONG("ERRMODE_EXCEPTION",	(zend_long)PDO_ERRMODE_EXCEPTION);

	REGISTER_PDO_CLASS_CONST_LONG("CASE_NATURAL",	(zend_long)PDO_CASE_NATURAL);
	REGISTER_PDO_CLASS_CONST_LONG("CASE_LOWER",	(zend_long)PDO_CASE_LOWER);
	REGISTER_PDO_CLASS_CONST_LONG("CASE_UPPER",	(zend_long)PDO_CASE_UPPER);

	REGISTER_PDO_CLASS_CONST_LONG("NULL_NATURAL",	(zend_long)PDO_NULL_NATURAL);
	REGISTER_PDO_CLASS_CONST_LONG("NULL_EMPTY_STRING",	(zend_long)PDO_NULL_EMPTY_STRING);
	REGISTER_PDO_CLASS_CONST_LONG("NULL_TO_STRING",	(zend_long)PDO_NULL_TO_STRING);

	REGISTER_PDO_CLASS_CONST_STRING("ERR_NONE",	PDO_ERR_NONE);

	REGISTER_PDO_CLASS_CONST_LONG("FETCH_ORI_NEXT", (zend_long)PDO_FETCH_ORI_NEXT);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_ORI_PRIOR", (zend_long)PDO_FETCH_ORI_PRIOR);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_ORI_FIRST", (zend_long)PDO_FETCH_ORI_FIRST);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_ORI_LAST", (zend_long)PDO_FETCH_ORI_LAST);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_ORI_ABS", (zend_long)PDO_FETCH_ORI_ABS);
	REGISTER_PDO_CLASS_CONST_LONG("FETCH_ORI_REL", (zend_long)PDO_FETCH_ORI_REL);

	REGISTER_PDO_CLASS_CONST_LONG("CURSOR_FWDONLY", (zend_long)PDO_CURSOR_FWDONLY);
	REGISTER_PDO_CLASS_CONST_LONG("CURSOR_SCROLL", (zend_long)PDO_CURSOR_SCROLL);
}

static void dbh_free(pdo_dbh_t *dbh, zend_bool free_persistent)
{
	int i;

	if (dbh->query_stmt) {
		zval_ptr_dtor(&dbh->query_stmt_zval);
		dbh->query_stmt = NULL;
	}

	if (dbh->is_persistent) {
#if ZEND_DEBUG
		ZEND_ASSERT(!free_persistent || (dbh->refcount == 1));
#endif
		if (!free_persistent && (--dbh->refcount)) {
			return;
		}
	}

	if (dbh->methods) {
		dbh->methods->closer(dbh);
	}

	if (dbh->data_source) {
		pefree((char *)dbh->data_source, dbh->is_persistent);
	}
	if (dbh->username) {
		pefree(dbh->username, dbh->is_persistent);
	}
	if (dbh->password) {
		pefree(dbh->password, dbh->is_persistent);
	}

	if (dbh->persistent_id) {
		pefree((char *)dbh->persistent_id, dbh->is_persistent);
	}

	if (!Z_ISUNDEF(dbh->def_stmt_ctor_args)) {
		zval_ptr_dtor(&dbh->def_stmt_ctor_args);
	}

	for (i = 0; i < PDO_DBH_DRIVER_METHOD_KIND__MAX; i++) {
		if (dbh->cls_methods[i]) {
			zend_hash_destroy(dbh->cls_methods[i]);
			pefree(dbh->cls_methods[i], dbh->is_persistent);
		}
	}

	pefree(dbh, dbh->is_persistent);
}

static void pdo_dbh_free_storage(zend_object *std)
{
	pdo_dbh_t *dbh = php_pdo_dbh_fetch_inner(std);
	if (dbh->driver_data && dbh->methods && dbh->methods->rollback && pdo_is_in_transaction(dbh)) {
		dbh->methods->rollback(dbh);
		dbh->in_txn = 0;
	}

	if (dbh->is_persistent && dbh->methods && dbh->methods->persistent_shutdown) {
		dbh->methods->persistent_shutdown(dbh);
	}
	zend_object_std_dtor(std);
	dbh_free(dbh, 0);
}

zend_object *pdo_dbh_new(zend_class_entry *ce)
{
	pdo_dbh_object_t *dbh;

	dbh = zend_object_alloc(sizeof(pdo_dbh_object_t), ce);
	zend_object_std_init(&dbh->std, ce);
	object_properties_init(&dbh->std, ce);
	rebuild_object_properties(&dbh->std);
	dbh->inner = ecalloc(1, sizeof(pdo_dbh_t));
	dbh->inner->def_stmt_ce = pdo_dbstmt_ce;

	dbh->std.handlers = &pdo_dbh_object_handlers;

	return &dbh->std;
}

/* }}} */

ZEND_RSRC_DTOR_FUNC(php_pdo_pdbh_dtor) /* {{{ */
{
	if (res->ptr) {
		pdo_dbh_t *dbh = (pdo_dbh_t*)res->ptr;
		dbh_free(dbh, 1);
		res->ptr = NULL;
	}
}
/* }}} */
