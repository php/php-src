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
#include "zend_attributes.h"
#include "zend_exceptions.h"
#include "zend_object_handlers.h"
#include "zend_hash.h"
#include "pdo_dbh_arginfo.h"
#include "zend_observer.h"
#include "zend_extensions.h"

static bool pdo_dbh_attribute_set(pdo_dbh_t *dbh, zend_long attr, zval *value);

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

void pdo_raise_impl_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, pdo_error_type sqlstate, const char *supp) /* {{{ */
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

	memcpy(*pdo_err, sqlstate, sizeof(pdo_error_type));

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
		zval *item;
		array_init(&info);

		add_next_index_string(&info, *pdo_err);

		dbh->methods->fetch_err(dbh, stmt, &info);

		if ((item = zend_hash_index_find(Z_ARRVAL(info), 1)) != NULL
				&& Z_TYPE_P(item) == IS_LONG) {
			native_code = Z_LVAL_P(item);
		}

		if ((item = zend_hash_index_find(Z_ARRVAL(info), 2)) != NULL) {
			supp = estrndup(Z_STRVAL_P(item), Z_STRLEN_P(item));
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

static bool create_driver_specific_pdo_object(pdo_driver_t *driver, zend_class_entry *called_scope, zval *new_object)
{
	zend_class_entry *ce;
	zend_class_entry *ce_based_on_driver_name = NULL, *ce_based_on_called_object = NULL;

	ce_based_on_driver_name = zend_hash_str_find_ptr(&pdo_driver_specific_ce_hash, driver->driver_name, driver->driver_name_len);

	ZEND_HASH_MAP_FOREACH_PTR(&pdo_driver_specific_ce_hash, ce) {
		if (called_scope != pdo_dbh_ce && instanceof_function(called_scope, ce)) {
			ce_based_on_called_object = called_scope;
			break;
		}
	} ZEND_HASH_FOREACH_END();

	if (ce_based_on_called_object) {
		if (ce_based_on_driver_name) {
			if (instanceof_function(ce_based_on_called_object, ce_based_on_driver_name) == false) {
				zend_throw_exception_ex(pdo_exception_ce, 0,
					"%s::connect() cannot be called when connecting to the \"%s\" driver, "
					"either %s::connect() or PDO::connect() must be called instead",
					ZSTR_VAL(called_scope->name), driver->driver_name, ZSTR_VAL(ce_based_on_driver_name->name));
				return false;
			}

			/* A driver-specific implementation was instantiated via the connect() method of the appropriate driver class */
			object_init_ex(new_object, ce_based_on_called_object);
			return true;
		} else {
			zend_throw_exception_ex(pdo_exception_ce, 0,
				"%s::connect() cannot be called when connecting to an unknown driver, "
				"PDO::connect() must be called instead",
				ZSTR_VAL(called_scope->name));
			return false;
		}
	}

	if (ce_based_on_driver_name) {
		if (called_scope != pdo_dbh_ce) {
			/* A driver-specific implementation was instantiated via the connect method of a wrong driver class */
			zend_throw_exception_ex(pdo_exception_ce, 0,
				"%s::connect() cannot be called when connecting to the \"%s\" driver, "
				"either %s::connect() or PDO::connect() must be called instead",
				ZSTR_VAL(called_scope->name), driver->driver_name, ZSTR_VAL(ce_based_on_driver_name->name));
			return false;
		}

		/* A driver-specific implementation was instantiated via PDO::__construct() */
		object_init_ex(new_object, ce_based_on_driver_name);
	} else {
		/* No driver-specific implementation found */
		object_init_ex(new_object, called_scope);
	}

	return true;
}

static void internal_construct(INTERNAL_FUNCTION_PARAMETERS, zend_object *object, zend_class_entry *current_scope, zval *new_zval_object)
{
	pdo_dbh_t *dbh = NULL;
	bool is_persistent = 0;
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

	if (new_zval_object != NULL) {
		ZEND_ASSERT((driver->driver_name != NULL) && "PDO driver name is null");
		if (!create_driver_specific_pdo_object(driver, current_scope, new_zval_object)) {
			RETURN_THROWS();
		}

		dbh = Z_PDO_DBH_P(new_zval_object);
	} else {
		dbh = php_pdo_dbh_fetch_inner(object);
	}

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
			php_pdo_dbh_fetch_object(object)->inner = pdbh;
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

	/* pdo_dbh_attribute_set() can emit a Warning if the ERR_MODE is set to warning
	 * As we are in a constructor we override the behaviour by replacing the error handler */
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

				/* TODO: Should the constructor fail when the attribute cannot be set? */
				pdo_dbh_attribute_set(dbh, long_key, attr_value);
			} ZEND_HASH_FOREACH_END();
		}

		zend_restore_error_handling(&zeh);
		return;
	}

	/* the connection failed; things will tidy up in free_storage */
	if (is_persistent) {
		dbh->refcount--;
	}

	/* XXX raise exception */
	zend_restore_error_handling(&zeh);
	if (!EG(exception)) {
		zend_throw_exception(pdo_exception_ce, "Constructor failed", 0);
	}
}

/* {{{ */
PHP_METHOD(PDO, __construct)
{
	internal_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, Z_OBJ(EX(This)), EX(This).value.ce, NULL);
}
/* }}} */

/* {{{ */
PHP_METHOD(PDO, connect)
{
	internal_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, Z_OBJ(EX(This)), EX(This).value.ce, return_value);
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

static void pdo_stmt_construct(pdo_stmt_t *stmt, zval *object, zend_class_entry *dbstmt_ce, HashTable *ctor_args)
{
	zval query_string;
	zend_string *key;

	ZVAL_STR(&query_string, stmt->query_string);
	key = ZSTR_INIT_LITERAL("queryString", 0);
	zend_std_write_property(Z_OBJ_P(object), key, &query_string, NULL);
	zend_string_release_ex(key, 0);

	if (dbstmt_ce->constructor) {
		zend_call_known_function(dbstmt_ce->constructor, Z_OBJ_P(object), Z_OBJCE_P(object), NULL, 0, NULL, ctor_args);
	}
}

/* {{{ Prepares a statement for execution and returns a statement object */
PHP_METHOD(PDO, prepare)
{
	pdo_stmt_t *stmt;
	zend_string *statement;
	zval *options = NULL, *value, *item, ctor_args;
	zend_class_entry *dbstmt_ce, *pce;
	pdo_dbh_object_t *dbh_obj = Z_PDO_OBJECT_P(ZEND_THIS);
	pdo_dbh_t *dbh = dbh_obj->inner;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(statement)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	PDO_CONSTRUCT_CHECK;

	if (ZSTR_LEN(statement) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	PDO_DBH_CLEAR_ERR();

	if (options && (value = zend_hash_index_find(Z_ARRVAL_P(options), PDO_ATTR_STATEMENT_CLASS)) != NULL) {
		if (Z_TYPE_P(value) != IS_ARRAY) {
			zend_type_error("PDO::ATTR_STATEMENT_CLASS value must be of type array, %s given",
				zend_zval_value_name(value));
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
					zend_zval_value_name(value));
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
	stmt->query_string = zend_string_copy(statement);
	stmt->default_fetch_type = dbh->default_fetch_type;
	stmt->dbh = dbh;
	/* give it a reference to me */
	ZVAL_OBJ_COPY(&stmt->database_object_handle, &dbh_obj->std);
	/* we haven't created a lazy object yet */
	ZVAL_UNDEF(&stmt->lazy_object_ref);

	if (dbh->methods->preparer(dbh, statement, stmt, options)) {
		if (Z_TYPE(ctor_args) == IS_ARRAY) {
			pdo_stmt_construct(stmt, return_value, dbstmt_ce, Z_ARRVAL(ctor_args));
		} else {
			pdo_stmt_construct(stmt, return_value, dbstmt_ce, /* ctor_args */ NULL);
		}
		return;
	}

	PDO_HANDLE_DBH_ERR();

	/* kill the object handle for the stmt here */
	zval_ptr_dtor(return_value);

	RETURN_FALSE;
}
/* }}} */


static bool pdo_is_in_transaction(pdo_dbh_t *dbh) {
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
		/* Throw an exception when the driver does not support transactions */
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "This driver doesn't support transactions");
		RETURN_THROWS();
	}

	if (dbh->methods->begin(dbh)) {
		dbh->in_txn = true;
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
		dbh->in_txn = false;
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
		dbh->in_txn = false;
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

PDO_API bool pdo_get_long_param(zend_long *lval, zval *value)
{
	switch (Z_TYPE_P(value)) {
		case IS_LONG:
		case IS_TRUE:
		case IS_FALSE:
			*lval = zval_get_long(value);
			return true;
		case IS_STRING:
			if (IS_LONG == is_numeric_str_function(Z_STR_P(value), lval, NULL)) {
				return true;
			}
			ZEND_FALLTHROUGH;
		default:
			zend_type_error("Attribute value must be of type int for selected attribute, %s given", zend_zval_value_name(value));
			return false;
	}
}
PDO_API bool pdo_get_bool_param(bool *bval, zval *value)
{
	switch (Z_TYPE_P(value)) {
		case IS_TRUE:
			*bval = true;
			return true;
		case IS_FALSE:
			*bval = false;
			return true;
		case IS_LONG:
			*bval = zval_is_true(value);
			return true;
		case IS_STRING: /* TODO Should string be allowed? */
		default:
			zend_type_error("Attribute value must be of type bool for selected attribute, %s given", zend_zval_value_name(value));
			return false;
	}
}

/* Return false on failure, true otherwise */
static bool pdo_dbh_attribute_set(pdo_dbh_t *dbh, zend_long attr, zval *value) /* {{{ */
{
	zend_long lval;
	bool bval;

	switch (attr) {
		case PDO_ATTR_ERRMODE:
			if (!pdo_get_long_param(&lval, value)) {
				return false;
			}
			switch (lval) {
				case PDO_ERRMODE_SILENT:
				case PDO_ERRMODE_WARNING:
				case PDO_ERRMODE_EXCEPTION:
					dbh->error_mode = lval;
					return true;
				default:
					zend_value_error("Error mode must be one of the PDO::ERRMODE_* constants");
					return false;
			}
			return false;

		case PDO_ATTR_CASE:
			if (!pdo_get_long_param(&lval, value)) {
				return false;
			}
			switch (lval) {
				case PDO_CASE_NATURAL:
				case PDO_CASE_UPPER:
				case PDO_CASE_LOWER:
					dbh->desired_case = lval;
					return true;
				default:
					zend_value_error("Case folding mode must be one of the PDO::CASE_* constants");
					return false;
			}
			return false;

		case PDO_ATTR_ORACLE_NULLS:
			if (!pdo_get_long_param(&lval, value)) {
				return false;
			}
			/* TODO Check for valid value (NULL_NATURAL, NULL_EMPTY_STRING, NULL_TO_STRING)? */
			dbh->oracle_nulls = lval;
			return true;

		case PDO_ATTR_DEFAULT_FETCH_MODE:
			if (Z_TYPE_P(value) == IS_ARRAY) {
				zval *tmp;
				if ((tmp = zend_hash_index_find(Z_ARRVAL_P(value), 0)) != NULL && Z_TYPE_P(tmp) == IS_LONG) {
					if (Z_LVAL_P(tmp) == PDO_FETCH_INTO || Z_LVAL_P(tmp) == PDO_FETCH_CLASS) {
						zend_value_error("PDO::FETCH_INTO and PDO::FETCH_CLASS cannot be set as the default fetch mode");
						return false;
					}
				}
				lval = zval_get_long(value);
			} else {
				if (!pdo_get_long_param(&lval, value)) {
					return false;
				}
			}
			if (lval == PDO_FETCH_USE_DEFAULT) {
				zend_value_error("Fetch mode must be a bitmask of PDO::FETCH_* constants");
				return false;
			}
			dbh->default_fetch_type = lval;
			return true;

		case PDO_ATTR_STRINGIFY_FETCHES:
			if (!pdo_get_bool_param(&bval, value)) {
				return false;
			}
			dbh->stringify = bval;
			if (dbh->methods->set_attribute) {
				dbh->methods->set_attribute(dbh, attr, value);
			}
			return true;

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
				return false;
			}
			if (Z_TYPE_P(value) != IS_ARRAY) {
				zend_type_error("PDO::ATTR_STATEMENT_CLASS value must be of type array, %s given",
					zend_zval_value_name(value));
				return false;
			}
			if ((item = zend_hash_index_find(Z_ARRVAL_P(value), 0)) == NULL) {
				zend_value_error("PDO::ATTR_STATEMENT_CLASS value must be an array with the format "
					"array(classname, constructor_args)");
				return false;
			}
			if (Z_TYPE_P(item) != IS_STRING || (pce = zend_lookup_class(Z_STR_P(item))) == NULL) {
				zend_type_error("PDO::ATTR_STATEMENT_CLASS class must be a valid class");
				return false;
			}
			if (!instanceof_function(pce, pdo_dbstmt_ce)) {
				zend_type_error("PDO::ATTR_STATEMENT_CLASS class must be derived from PDOStatement");
				return false;
			}
			if (pce->constructor && !(pce->constructor->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED))) {
				zend_type_error("User-supplied statement class cannot have a public constructor");
				return false;
			}
			dbh->def_stmt_ce = pce;
			if (!Z_ISUNDEF(dbh->def_stmt_ctor_args)) {
				zval_ptr_dtor(&dbh->def_stmt_ctor_args);
				ZVAL_UNDEF(&dbh->def_stmt_ctor_args);
			}
			if ((item = zend_hash_index_find(Z_ARRVAL_P(value), 1)) != NULL) {
				if (Z_TYPE_P(item) != IS_ARRAY) {
					zend_type_error("PDO::ATTR_STATEMENT_CLASS constructor_args must be of type ?array, %s given",
						zend_zval_value_name(value));
					return false;
				}
				ZVAL_COPY(&dbh->def_stmt_ctor_args, item);
			}
			return true;
		}
		/* Don't throw a ValueError as the attribute might be a driver specific one */
		default:;
	}

	if (!dbh->methods->set_attribute) {
		goto fail;
	}

	PDO_DBH_CLEAR_ERR();
	if (dbh->methods->set_attribute(dbh, attr, value)) {
		return true;
	}

fail:
	if (!dbh->methods->set_attribute) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support setting attributes");
	} else {
		PDO_HANDLE_DBH_ERR();
	}
	return false;
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

	RETURN_BOOL(pdo_dbh_attribute_set(dbh, attr, value));
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

		case PDO_ATTR_STRINGIFY_FETCHES:
			RETURN_BOOL(dbh->stringify);

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
			/* No error state, just return as the return_value has been assigned
			 * by the get_attribute handler */
			return;
	}
}
/* }}} */

/* {{{ Execute a statement that does not return a row set, returning the number of affected rows */
PHP_METHOD(PDO, exec)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);
	zend_string *statement;
	zend_long ret;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(statement)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(statement) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	PDO_DBH_CLEAR_ERR();
	PDO_CONSTRUCT_CHECK;
	ret = dbh->methods->doer(dbh, statement);
	if (ret == -1) {
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
	zend_string *name = NULL;
	zend_string *last_id = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	PDO_CONSTRUCT_CHECK;

	PDO_DBH_CLEAR_ERR();

	if (!dbh->methods->last_id) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support lastInsertId()");
		RETURN_FALSE;
	}
	last_id = dbh->methods->last_id(dbh, name);
	if (!last_id) {
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}
	RETURN_STR(last_id);
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
	zend_string *statement;
	zend_long fetch_mode;
	bool fetch_mode_is_null = 1;
	zval *args = NULL;
	uint32_t num_args = 0;
	pdo_dbh_object_t *dbh_obj = Z_PDO_OBJECT_P(ZEND_THIS);
	pdo_dbh_t *dbh = dbh_obj->inner;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "S|l!*", &statement,
			&fetch_mode, &fetch_mode_is_null, &args, &num_args)) {
		RETURN_THROWS();
	}

	PDO_CONSTRUCT_CHECK;

	if (ZSTR_LEN(statement) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	PDO_DBH_CLEAR_ERR();

	if (!pdo_stmt_instantiate(dbh, return_value, dbh->def_stmt_ce, &dbh->def_stmt_ctor_args)) {
		RETURN_THROWS();
	}
	stmt = Z_PDO_STMT_P(return_value);

	/* unconditionally keep this for later reference */
	stmt->query_string = zend_string_copy(statement);
	stmt->active_query_string = zend_string_copy(stmt->query_string);
	stmt->default_fetch_type = dbh->default_fetch_type;
	stmt->dbh = dbh;
	/* give it a reference to me */
	ZVAL_OBJ_COPY(&stmt->database_object_handle, &dbh_obj->std);
	/* we haven't created a lazy object yet */
	ZVAL_UNDEF(&stmt->lazy_object_ref);

	if (dbh->methods->preparer(dbh, statement, stmt, NULL)) {
		PDO_STMT_CLEAR_ERR();
		if (fetch_mode_is_null || pdo_stmt_setup_fetch_mode(stmt, fetch_mode, 2, args, num_args)) {
			/* now execute the statement */
			PDO_STMT_CLEAR_ERR();
			if (stmt->methods->executer(stmt)) {
				bool ret = true;
				if (!stmt->executed) {
					if (stmt->dbh->alloc_own_columns) {
						ret = pdo_stmt_describe_columns(stmt);
					}
					stmt->executed = 1;
				}
				if (ret) {
					if (Z_TYPE(dbh->def_stmt_ctor_args) == IS_ARRAY) {
						pdo_stmt_construct(stmt, return_value, dbh->def_stmt_ce, Z_ARRVAL(dbh->def_stmt_ctor_args));
					} else {
						pdo_stmt_construct(stmt, return_value, dbh->def_stmt_ce, /* ctor_args */ NULL);
					}
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

/* {{{ quotes string for use in a query.
 * The optional paramtype acts as a hint for drivers that have alternate quoting styles.
 * The default value is PDO_PARAM_STR */
PHP_METHOD(PDO, quote)
{
	pdo_dbh_t *dbh = Z_PDO_DBH_P(ZEND_THIS);
	zend_string *str, *quoted;
	zend_long paramtype = PDO_PARAM_STR;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(paramtype)
	ZEND_PARSE_PARAMETERS_END();

	PDO_CONSTRUCT_CHECK;

	PDO_DBH_CLEAR_ERR();
	if (!dbh->methods->quoter) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support quoting");
		RETURN_FALSE;
	}
	quoted = dbh->methods->quoter(dbh, str, paramtype);

	if (quoted == NULL) {
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}

	RETURN_STR(quoted);
}
/* }}} */

/* {{{ Return array of available PDO drivers */
PHP_METHOD(PDO, getAvailableDrivers)
{
	pdo_driver_t *pdriver;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	ZEND_HASH_MAP_FOREACH_PTR(&pdo_driver_hash, pdriver) {
		add_next_index_stringl(return_value, (char*)pdriver->driver_name, pdriver->driver_name_len);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static void cls_method_dtor(zval *el) /* {{{ */ {
	zend_function *func = (zend_function*)Z_PTR_P(el);
	if (func->common.function_name) {
		zend_string_release_ex(func->common.function_name, 0);
	}
	if (ZEND_MAP_PTR(func->common.run_time_cache)) {
		efree(ZEND_MAP_PTR(func->common.run_time_cache));
	}
	efree(func);
}
/* }}} */

static void cls_method_pdtor(zval *el) /* {{{ */ {
	zend_function *func = (zend_function*)Z_PTR_P(el);
	if (func->common.function_name) {
		zend_string_release_ex(func->common.function_name, 1);
	}
	if (ZEND_MAP_PTR(func->common.run_time_cache)) {
		pefree(ZEND_MAP_PTR(func->common.run_time_cache), 1);
	}
	pefree(func, 1);
}
/* }}} */

/* {{{ overloaded object handlers for PDO class */
bool pdo_hash_methods(pdo_dbh_object_t *dbh_obj, int kind)
{
	const zend_function_entry *funcs;
	zend_internal_function func;
	size_t namelen;
	char *lc_name;
	pdo_dbh_t *dbh = dbh_obj->inner;

	if (!dbh || !dbh->methods || !dbh->methods->get_driver_methods) {
		return false;
	}
	funcs =	dbh->methods->get_driver_methods(dbh, kind);
	if (!funcs) {
		return false;
	}

	dbh->cls_methods[kind] = pemalloc(sizeof(HashTable), dbh->is_persistent);
	zend_hash_init(dbh->cls_methods[kind], 8, NULL,
			dbh->is_persistent? cls_method_pdtor : cls_method_dtor, dbh->is_persistent);

	memset(&func, 0, sizeof(func));

	size_t rt_cache_size = zend_internal_run_time_cache_reserved_size();
	while (funcs->fname) {
		func.type = ZEND_INTERNAL_FUNCTION;
		func.handler = funcs->handler;
		func.function_name = zend_string_init(funcs->fname, strlen(funcs->fname), dbh->is_persistent);
		func.scope = dbh_obj->std.ce;
		func.prototype = NULL;
		ZEND_MAP_PTR(func.run_time_cache) = rt_cache_size ? pecalloc(rt_cache_size, 1, dbh->is_persistent) : NULL;
		func.T = ZEND_OBSERVER_ENABLED;
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

	return true;
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

static HashTable *dbh_get_gc(zend_object *object, zval **gc_data, int *gc_count)
{
	pdo_dbh_t *dbh = php_pdo_dbh_fetch_inner(object);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	zend_get_gc_buffer_add_zval(gc_buffer, &dbh->def_stmt_ctor_args);
	if (dbh->methods && dbh->methods->get_gc) {
		dbh->methods->get_gc(dbh, gc_buffer);
	}
	zend_get_gc_buffer_use(gc_buffer, gc_data, gc_count);
	return zend_std_get_properties(object);
}

static zend_object_handlers pdo_dbh_object_handlers;

static void pdo_dbh_free_storage(zend_object *std);

void pdo_dbh_init(int module_number)
{
	pdo_dbh_ce = register_class_PDO();
	pdo_dbh_ce->create_object = pdo_dbh_new;
	pdo_dbh_ce->default_object_handlers = &pdo_dbh_object_handlers;

	memcpy(&pdo_dbh_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_dbh_object_handlers.offset = XtOffsetOf(pdo_dbh_object_t, std);
	pdo_dbh_object_handlers.free_obj = pdo_dbh_free_storage;
	pdo_dbh_object_handlers.clone_obj = NULL;
	pdo_dbh_object_handlers.get_method = dbh_method_get;
	pdo_dbh_object_handlers.compare = zend_objects_not_comparable;
	pdo_dbh_object_handlers.get_gc = dbh_get_gc;
}

static void dbh_free(pdo_dbh_t *dbh, bool free_persistent)
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

	/* dbh might be null if we OOMed during object initialization. */
	if (!dbh) {
		return;
	}

	if (dbh->driver_data && dbh->methods && dbh->methods->rollback && pdo_is_in_transaction(dbh)) {
		dbh->methods->rollback(dbh);
		dbh->in_txn = false;
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
