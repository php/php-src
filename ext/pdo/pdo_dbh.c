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

void pdo_handle_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_error_type *pdo_err = &dbh->error_code;
	const char *msg = "<<Unknown>>";
	char *supp = NULL;
	long native_code = 0;
	char *message = NULL;
	zval *info = NULL;

	if (dbh->error_mode == PDO_ERRMODE_SILENT) {
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

	if (dbh->methods->fetch_err) {
		
		MAKE_STD_ZVAL(info);
		array_init(info);

		add_next_index_string(info, *pdo_err, 1);
		
		if (dbh->methods->fetch_err(dbh, stmt, info TSRMLS_CC)) {
			zval **item;

			if (SUCCESS == zend_hash_index_find(Z_ARRVAL_P(info), 1, (void**)&item)) {
				native_code = Z_LVAL_PP(item);
			}
			
			if (SUCCESS == zend_hash_index_find(Z_ARRVAL_P(info), 2, (void**)&item)) {
				supp = estrndup(Z_STRVAL_PP(item), Z_STRLEN_PP(item));
			}
		}
	}

	if (supp) {
		spprintf(&message, 0, "SQLSTATE[%s]: %s: %ld %s", *pdo_err, msg, native_code, supp);
	} else {
		spprintf(&message, 0, "SQLSTATE[%s]: %s", *pdo_err, msg);
	}

	if (dbh->error_mode == PDO_ERRMODE_WARNING) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", message);

		if (info) {
			zval_ptr_dtor(&info);
		}
	} else {
		zval *ex;
		zend_class_entry *def_ex = zend_exception_get_default(), *pdo_ex = php_pdo_get_exception();

		MAKE_STD_ZVAL(ex);
		object_init_ex(ex, pdo_ex);

		zend_update_property_string(def_ex, ex, "message", sizeof("message")-1, message TSRMLS_CC);
		zend_update_property_string(def_ex, ex, "code", sizeof("code")-1, *pdo_err TSRMLS_CC);
		
		if (info) {
			zend_update_property(pdo_ex, ex, "errorInfo", sizeof("errorInfo")-1, info TSRMLS_CC);
		}

		zend_throw_exception_object(ex TSRMLS_CC);
	}
	
	if (message) {
		efree(message);
	}

	if (supp) {
		efree(supp);
	}
}

static char *dsn_from_uri(char *uri, char *buf, size_t buflen TSRMLS_DC)
{
	php_stream *stream;
	char *dsn = NULL;

	stream = php_stream_open_wrapper(uri, "rb", ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);
	if (stream) {
		dsn = php_stream_get_line(stream, buf, buflen, NULL);
		php_stream_close(stream);
	}
	return dsn;
}

/* {{{ proto object PDO::__construct(string dsn, string username, string passwd [, array driver_opts])
   */
static PHP_FUNCTION(dbh_constructor)
{
	zval *object = getThis();
	pdo_dbh_t *dbh = NULL;
	zend_bool is_persistent = FALSE;
	char *data_source;
	int data_source_len;
	char *colon;
	char *username=NULL, *password=NULL;
	int usernamelen, passwordlen;
	pdo_driver_t *driver = NULL;
	zval *driver_options = NULL;
	char alt_dsn[512];

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ssa!", &data_source, &data_source_len,
				&username, &usernamelen, &password, &passwordlen, &driver_options)) {
		ZVAL_NULL(object);
		return;
	}

	/* parse the data source name */
	colon = strchr(data_source, ':');

	if (!colon) {
		/* let's see if this string has a matching dsn in the php.ini */
		char *ini_dsn = NULL;

		snprintf(alt_dsn, sizeof(alt_dsn), "pdo.dsn.%s", data_source);
		if (FAILURE == cfg_get_string(alt_dsn, &ini_dsn)) {
			zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "invalid data source name");
			ZVAL_NULL(object);
			return;
		}

		data_source = ini_dsn;
		colon = strchr(data_source, ':');
		
		if (!colon) {
			zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "invalid data source name (via INI: %s)", alt_dsn);
			ZVAL_NULL(object);
			return;
		}
	}

	if (!strncmp(data_source, "uri:", sizeof("uri:")-1)) {
		/* the specified URI holds connection details */
		data_source = dsn_from_uri(data_source, alt_dsn, sizeof(alt_dsn) TSRMLS_CC);
		if (!data_source) {
			zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "invalid data source URI");
			ZVAL_NULL(object);
			return;
		}
		colon = strchr(data_source, ':');
		if (!colon) {
			zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "invalid data source name (via URI)");
			ZVAL_NULL(object);
			return;
		}
	}

	driver = pdo_find_driver(data_source, colon - data_source);

	if (!driver) {
		/* NB: don't want to include the data_source in the error message as
		 * it might contain a password */
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "could not find driver");
		ZVAL_NULL(object);
		return;
	}
	
	dbh = (pdo_dbh_t *) zend_object_store_get_object(object TSRMLS_CC);

	/* is this supposed to be a persistent connection ? */
	if (driver_options) {
		zval **v;
		int plen;
		char *hashkey = NULL;
		list_entry *le;
		pdo_dbh_t *pdbh = NULL;

		if (SUCCESS == zend_hash_index_find(Z_ARRVAL_P(driver_options), PDO_ATTR_PERSISTENT, (void**)&v)) {
			if (Z_TYPE_PP(v) == IS_STRING) {
				/* user specified key */
				plen = spprintf(&hashkey, 0, "PDO:DBH:DSN=%s:%s:%s:%s", data_source,
						username ? username : "",
						password ? password : "",
						Z_STRVAL_PP(v));
				is_persistent = 1;
			} else {
				convert_to_long_ex(v);
				is_persistent = Z_LVAL_PP(v) ? 1 : 0;
				plen = spprintf(&hashkey, 0, "PDO:DBH:DSN=%s:%s:%s", data_source,
						username ? username : "",
						password ? password : "");
			}
		}

		/* let's see if we have one cached.... */
		if (is_persistent && SUCCESS == zend_hash_find(&EG(persistent_list), hashkey, plen+1, (void*)&le)) {
			if (Z_TYPE_P(le) == php_pdo_list_entry()) {
				pdbh = (pdo_dbh_t*)le->ptr;

				/* is the connection still alive ? */
				if (pdbh->methods->check_liveness && FAILURE == (pdbh->methods->check_liveness)(pdbh TSRMLS_CC)) {
					/* nope... need to kill it */
					pdbh = NULL;
				}
			}
		}

		if (is_persistent && !pdbh) {
			/* need a brand new pdbh */
			pdbh = pecalloc(1, sizeof(*pdbh), 1);

			if (!pdbh) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "out of memory while allocating PDO handle");
				/* NOTREACHED */
			}
			
			pdbh->is_persistent = 1;
			pdbh->persistent_id = pemalloc(plen + 1, 1);
			memcpy((char *)pdbh->persistent_id, hashkey, plen+1);
			pdbh->persistent_id_len = plen+1;
			pdbh->refcount = 1;
		}

		if (pdbh) {
			/* let's copy the emalloc bits over from the other handle */
			pdbh->ce = dbh->ce;
			pdbh->properties = dbh->properties;
			/* kill the non-persistent thingamy */
			efree(dbh);
			/* switch over to the persistent one */
			dbh = pdbh;
			zend_object_store_set_object(object, dbh TSRMLS_CC);
			dbh->refcount++;
		}

		if (hashkey) {
			efree(hashkey);
		}
	}
	
	dbh->data_source_len = strlen(colon + 1);
	dbh->data_source = (const char*)pestrdup(colon + 1, is_persistent);
	dbh->username = username ? pestrdup(username, is_persistent) : NULL;
	dbh->password = password ? pestrdup(password, is_persistent) : NULL;

	dbh->auto_commit = pdo_attr_lval(driver_options, PDO_ATTR_AUTOCOMMIT, 1 TSRMLS_CC);

	if (!dbh->data_source || (username && !dbh->username) || (password && !dbh->password)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "out of memory");
	}
	
	if (driver->db_handle_factory(dbh, driver_options TSRMLS_CC)) {
		/* all set */

		if (is_persistent) {
			list_entry le;

			/* register in the persistent list etc. */
			/* we should also need to replace the object store entry,
			   since it was created with emalloc */

			le.type = php_pdo_list_entry();
			le.ptr = dbh;

			if (FAILURE == zend_hash_update(&EG(persistent_list),
					(char*)dbh->persistent_id, dbh->persistent_id_len, (void*)&le,
					sizeof(le), NULL)) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to register persistent entry");
			}
		}
		return;	
	}

	/* the connection failed; things will tidy up in free_storage */
	/* XXX raise exception */
	ZVAL_NULL(object);
}
/* }}} */

/* {{{ proto object PDO::prepare(string statment [, int options [, array driver_options]])
   Prepares a statement for execution and returns a statement object */
/* TODO: options will be a PDO specific bitmask controlling such things as
 * cursor type. */
static PHP_METHOD(PDO, prepare)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	pdo_stmt_t *stmt;
	char *statement;
	int statement_len;
	zval *driver_options = NULL;
	long options = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|la", &statement,
			&statement_len, &options, &driver_options)) {
		RETURN_FALSE;
	}
	
	PDO_DBH_CLEAR_ERR();
	stmt = ecalloc(1, sizeof(*stmt));
	/* unconditionally keep this for later reference */
	stmt->query_string = estrndup(statement, statement_len);
	stmt->query_stringlen = statement_len;
	stmt->default_fetch_type = PDO_FETCH_BOTH;

	if (dbh->methods->preparer(dbh, statement, statement_len, stmt, options, driver_options TSRMLS_CC)) {
		/* prepared; create a statement object for PHP land to access it */
		Z_TYPE_P(return_value) = IS_OBJECT;
		Z_OBJ_HANDLE_P(return_value) = zend_objects_store_put(stmt, NULL, pdo_dbstmt_free_storage, NULL TSRMLS_CC);
		Z_OBJ_HT_P(return_value) = &pdo_dbstmt_object_handlers;

		/* give it a reference to me */
		stmt->database_object_handle = *getThis();
		zend_objects_store_add_ref(getThis() TSRMLS_CC);
		stmt->dbh = dbh;

		/* we haven't created a lazy object yet */
		ZVAL_NULL(&stmt->lazy_object_ref);

		stmt->refcount = 1;
		return;
	}
	efree(stmt);
	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool PDO::beginTransaction()
   Initiates a transaction */
static PHP_METHOD(PDO, beginTransaction)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (dbh->in_txn) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "There is already an active transaction");
		RETURN_FALSE;
	}
	
	if (!dbh->methods->begin) {
		/* TODO: this should be an exception; see the auto-commit mode
		 * comments below */
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "This driver doesn't support transactions");
		RETURN_FALSE;
	}

	if (dbh->methods->begin(dbh TSRMLS_CC)) {
		dbh->in_txn = 1;
		RETURN_TRUE;
	}

	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool PDO::commit()
   Commit a transaction */
static PHP_METHOD(PDO, commit)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!dbh->in_txn) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "There is no active transaction");
		RETURN_FALSE;
	}

	if (dbh->methods->commit(dbh TSRMLS_CC)) {
		dbh->in_txn = 0;
		RETURN_TRUE;
	}
	
	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool PDO::rollBack()
   roll back a transaction */
static PHP_METHOD(PDO, rollBack)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!dbh->in_txn) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "There is no active transaction");
		RETURN_FALSE;
	}

	if (dbh->methods->rollback(dbh TSRMLS_CC)) {
		dbh->in_txn = 0;
		RETURN_TRUE;
	}
		
	PDO_HANDLE_DBH_ERR();
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool PDO::setAttribute(long attribute, mixed value)
   Set an attribute */
static PHP_METHOD(PDO, setAttribute)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	long attr;
	zval *value = NULL;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz!", &attr, &value)) {
		RETURN_FALSE;
	}

	switch (attr) {
		case PDO_ATTR_ERRMODE:
			convert_to_long(value);
			switch (Z_LVAL_P(value)) {
				case PDO_ERRMODE_SILENT:
				case PDO_ERRMODE_WARNING:
				case PDO_ERRMODE_EXCEPTION:
					dbh->error_mode = Z_LVAL_P(value);
					RETURN_TRUE;
				default:
					zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "Error mode %d is invalid", Z_LVAL_P(value));
			}
			RETURN_FALSE;

		case PDO_ATTR_CASE:
			convert_to_long(value);
			switch (Z_LVAL_P(value)) {
				case PDO_CASE_NATURAL:
				case PDO_CASE_UPPER:
				case PDO_CASE_LOWER:
					dbh->desired_case = Z_LVAL_P(value);
					RETURN_TRUE;
				default:
					zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "Case folding mode %d is invalid", Z_LVAL_P(value));
			}
			RETURN_FALSE;

		case PDO_ATTR_ORACLE_NULLS:
			convert_to_long(value);
			dbh->oracle_nulls = Z_LVAL_P(value) ? 1 : 0;
			RETURN_TRUE;
			
		default:
			;
	}

	if (!dbh->methods->set_attribute) {
		goto fail;
	}

	PDO_DBH_CLEAR_ERR();
	if (dbh->methods->set_attribute(dbh, attr, value TSRMLS_CC)) {
		RETURN_TRUE;
	}

fail:
	if (attr == PDO_ATTR_AUTOCOMMIT) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "The auto-commit mode cannot be changed for this driver");
	} else if (!dbh->methods->set_attribute) {
		/* XXX: do something better here */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This driver doesn't support setting attributes");
	} else {
		PDO_HANDLE_DBH_ERR();
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed PDO::getAttribute(long attribute)
   Get an attribute */
static PHP_METHOD(PDO, getAttribute)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	long attr;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &attr)) {
		RETURN_FALSE;
	}

	if (!dbh->methods->get_attribute) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This driver doesn't support fetching attributes");
		RETURN_FALSE;
	}

	PDO_DBH_CLEAR_ERR();
	switch (dbh->methods->get_attribute(dbh, attr, return_value TSRMLS_CC)) {
		case -1:
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;

		case 0:
			/* XXX: should do something better here */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "This driver doesn't support fetching %ld attribute", attr);
			break;

		default:
			return;
	}
}
/* }}} */

/* {{{ proto long PDO::exec(string query)
   Execute a query that does not return a row set, returning the number of affected rows */
static PHP_METHOD(PDO, exec)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	char *statement;
	int statement_len;
	long ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &statement, &statement_len)) {
		RETURN_FALSE;
	}

	if (!statement_len) {
		RETURN_FALSE;
	}
	PDO_DBH_CLEAR_ERR();
	ret = dbh->methods->doer(dbh, statement, statement_len TSRMLS_CC);
	if(ret == -1) {
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */


/* {{{ proto int PDO::lastInsertId()
   Returns the number id of rows that we affected by the last call to PDO::exec().  Not always meaningful. */
static PHP_METHOD(PDO, lastInsertId)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS()) {
		RETURN_FALSE;
	}

	PDO_DBH_CLEAR_ERR();
	if (!dbh->methods->last_id) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This driver does not support last inserted id retrieval.");
	} else {
		RETURN_LONG(dbh->methods->last_id(dbh TSRMLS_CC));
	}
}
/* }}} */

/* {{{ proto string PDO::errorCode()
   Fetch the error code associated with the last operation on the database handle */
static PHP_METHOD(PDO, errorCode)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS()) {
		RETURN_FALSE;
	}

	RETURN_STRING(dbh->error_code, 1);
}
/* }}} */

/* {{{ proto int PDO::errorInfo()
   Fetch extended error information associated with the last operation on the database handle */
static PHP_METHOD(PDO, errorInfo)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (ZEND_NUM_ARGS()) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_next_index_string(return_value, dbh->error_code, 1);

	if (dbh->methods->fetch_err) {
		dbh->methods->fetch_err(dbh, NULL, return_value TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto object PDO::query(string sql [, PDOStatement::setFetchMode() args])
   Prepare and execute $sql; returns the statement object for iteration */
static PHP_METHOD(PDO, query)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	pdo_stmt_t *stmt;
	char *statement;
	int statement_len;
	zval *driver_options = NULL;
	long options = 0;

	if (FAILURE == zend_parse_parameters(1 TSRMLS_CC, "s", &statement,
			&statement_len)) {
		RETURN_FALSE;
	}
	
	PDO_DBH_CLEAR_ERR();
	stmt = ecalloc(1, sizeof(*stmt));
	/* unconditionally keep this for later reference */
	stmt->query_string = estrndup(statement, statement_len);
	stmt->query_stringlen = statement_len;
	stmt->default_fetch_type = PDO_FETCH_BOTH;

	if (dbh->methods->preparer(dbh, statement, statement_len, stmt, options, driver_options TSRMLS_CC)) {
		/* prepared; create a statement object for PHP land to access it */
		Z_TYPE_P(return_value) = IS_OBJECT;
		Z_OBJ_HANDLE_P(return_value) = zend_objects_store_put(stmt, NULL, pdo_dbstmt_free_storage, NULL TSRMLS_CC);
		Z_OBJ_HT_P(return_value) = &pdo_dbstmt_object_handlers;

		/* give it a reference to me */
		stmt->database_object_handle = *getThis();
		zend_objects_store_add_ref(getThis() TSRMLS_CC);
		stmt->dbh = dbh;

		/* we haven't created a lazy object yet */
		ZVAL_NULL(&stmt->lazy_object_ref);

		stmt->refcount = 1;

		if (ZEND_NUM_ARGS() == 1 ||
				SUCCESS == pdo_stmt_setup_fetch_mode(INTERNAL_FUNCTION_PARAM_PASSTHRU,
				stmt, 1)) {
			PDO_STMT_CLEAR_ERR();

			/* now execute the statement */
			PDO_STMT_CLEAR_ERR();
			if (stmt->methods->executer(stmt TSRMLS_CC)) {
				int ret = 1;
				if (!stmt->executed) {
					if (stmt->dbh->alloc_own_columns) {
						ret = pdo_stmt_describe_columns(stmt TSRMLS_CC);
					}
					stmt->executed = 1;
				}
				if (ret) {
					return;
				}
			}
		}
		/* something broke */
		PDO_HANDLE_STMT_ERR();
		
		/* TODO: kill the object handle for the stmt here */
	} else {
		efree(stmt);
		PDO_HANDLE_DBH_ERR();
	}
	RETURN_FALSE;
}
/* }}} */

function_entry pdo_dbh_functions[] = {
	PHP_ME_MAPPING(__construct, dbh_constructor,	NULL)
	PHP_ME(PDO, prepare, 		NULL, 					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, beginTransaction,NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, commit,			NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, rollBack,		NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, setAttribute,	NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, exec,			NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, query,			NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, lastInsertId,	NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, errorCode,		NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, errorInfo,		NULL,					ZEND_ACC_PUBLIC)
	PHP_ME(PDO, getAttribute,	NULL,					ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ overloaded object handlers for PDO class */
static zval *dbh_prop_read(zval *object, zval *member, int type TSRMLS_DC)
{
	zval *return_value;

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	return return_value;
}

static void dbh_prop_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	return;
}

static zval *dbh_read_dim(zval *object, zval *offset, int type TSRMLS_DC)
{
	zval *return_value;

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);

	return return_value;
}

static void dbh_write_dim(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	return;
}

static int dbh_prop_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	return 0;
}

static int dbh_dim_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	return 0;
}

static void dbh_prop_delete(zval *object, zval *offset TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete properties from a PDO DBH");
}

static void dbh_dim_delete(zval *object, zval *offset TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot delete dimensions from a PDO DBH");
}

static HashTable *dbh_get_properties(zval *object TSRMLS_DC)
{
	return NULL;
}

int pdo_hash_methods(pdo_dbh_t *dbh, int kind TSRMLS_DC)
{
	function_entry *funcs;
	zend_function func;
	zend_internal_function *ifunc = (zend_internal_function*)&func;
	int namelen;
	char *lc_name;

	if (!dbh->methods->get_driver_methods) {
		return 0;
	}
	funcs =	dbh->methods->get_driver_methods(dbh,
			PDO_DBH_DRIVER_METHOD_KIND_DBH TSRMLS_DC);
	if (!funcs) {
		return 0;
	}

	dbh->cls_methods[kind] = pemalloc(sizeof(HashTable), dbh->is_persistent);
	zend_hash_init_ex(dbh->cls_methods[kind], 8, NULL, NULL, dbh->is_persistent, 0);

	while (funcs->fname) {
		ifunc->handler = funcs->handler;
		ifunc->function_name = funcs->fname;
		ifunc->scope = dbh->ce;
		ifunc->prototype = NULL;
		if (funcs->arg_info) {
			ifunc->arg_info = funcs->arg_info + 1;
			ifunc->num_args = funcs->num_args;
			if (funcs->arg_info[0].required_num_args == -1) {
				ifunc->required_num_args = funcs->num_args;
			} else {
				ifunc->required_num_args = funcs->arg_info[0].required_num_args;
			}
			ifunc->pass_rest_by_reference = funcs->arg_info[0].pass_by_reference;
			ifunc->return_reference = funcs->arg_info[0].return_reference;
		} else {
			ifunc->arg_info = NULL;
			ifunc->num_args = 0;
			ifunc->required_num_args = 0;
			ifunc->pass_rest_by_reference = 0;
			ifunc->return_reference = 0;
		}
		if (funcs->flags) {
			ifunc->fn_flags = funcs->flags;
		} else {
			ifunc->fn_flags = ZEND_ACC_PUBLIC;
		}
		namelen = strlen(funcs->fname);
		lc_name = emalloc(namelen+1);
		zend_str_tolower_copy(lc_name, funcs->fname, namelen);
		zend_hash_add(dbh->cls_methods[kind], lc_name, namelen+1, &func, sizeof(func), NULL);
		efree(lc_name);
		funcs++;
	}

	return 1;
}

static union _zend_function *dbh_method_get(
#if PHP_API_VERSION >= 20041225
	zval **object_pp,
#else
	zval *object,
#endif
	char *method_name, int method_len TSRMLS_DC)
{
	zend_function *fbc = NULL;
	char *lc_method_name;
#if PHP_API_VERSION >= 20041225
	zval *object = *object_pp;
#endif
	pdo_dbh_t *dbh = zend_object_store_get_object(object TSRMLS_CC);

	lc_method_name = emalloc(method_len + 1);
	zend_str_tolower_copy(lc_method_name, method_name, method_len);

	if (zend_hash_find(&dbh->ce->function_table, lc_method_name, method_len+1, (void**)&fbc) == FAILURE) {

		/* not a pre-defined method, nor a user-defined method; check
		 * the driver specific methods */
		if (!dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH]) {
			if (!pdo_hash_methods(dbh, PDO_DBH_DRIVER_METHOD_KIND_DBH TSRMLS_CC)) {
				goto out;
			}
		}

		if (zend_hash_find(dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH],
				lc_method_name, method_len+1, (void**)&fbc) == FAILURE) {
			fbc = NULL;
			goto out;
		}
		/* got it */
	}
	
out:
	efree(lc_method_name);
	return fbc;
}

static int dbh_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	return FAILURE;
}


static union _zend_function *dbh_get_ctor(zval *object TSRMLS_DC)
{
 	static zend_internal_function ctor = {0};
	ctor.type = ZEND_INTERNAL_FUNCTION;
	ctor.function_name = "__construct";
	ctor.scope = pdo_dbh_ce;
	ctor.handler = ZEND_FN(dbh_constructor);

	return (union _zend_function*)&ctor;
}

static zend_class_entry *dbh_get_ce(zval *object TSRMLS_DC)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(object TSRMLS_CC);
	return dbh->ce;
}

static int dbh_get_classname(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(object TSRMLS_CC);

	*class_name = estrndup("PDO", sizeof("PDO")-1);
	*class_name_len = sizeof("PDO")-1;
	return 0;
}

static int dbh_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	return -1;
}

static zend_object_handlers pdo_dbh_object_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	dbh_prop_read,
	dbh_prop_write,
	dbh_read_dim,
	dbh_write_dim,
	NULL,
	NULL,
	NULL,
	dbh_prop_exists,
	dbh_prop_delete,
	dbh_dim_exists,
	dbh_dim_delete,
	dbh_get_properties,
	dbh_method_get,
	dbh_call_method,
	dbh_get_ctor,
	dbh_get_ce,
	dbh_get_classname,
	dbh_compare,
	NULL, /* cast */
	NULL
};

static void dbh_free(pdo_dbh_t *dbh TSRMLS_DC)
{
	if (--dbh->refcount)
		return;

	if (dbh->methods) {
		dbh->methods->closer(dbh TSRMLS_CC);
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

	pefree(dbh, dbh->is_persistent);
}

static void pdo_dbh_free_storage(zend_object *object TSRMLS_DC)
{
	pdo_dbh_t *dbh = (pdo_dbh_t*)object;
	if (!dbh) {
		return;
	}

	if (dbh->properties) {
		zend_hash_destroy(dbh->properties);
		efree(dbh->properties);
		dbh->properties = NULL;
	}

	if (!dbh->is_persistent) {
		dbh_free(dbh TSRMLS_CC);
	}
}

zend_object_value pdo_dbh_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	pdo_dbh_t *dbh;
	dbh = emalloc(sizeof(*dbh));
	memset(dbh, 0, sizeof(*dbh));
	dbh->ce = ce;
	dbh->refcount = 1;
	ALLOC_HASHTABLE(dbh->properties);
	zend_hash_init(dbh->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	retval.handle = zend_objects_store_put(dbh, NULL, pdo_dbh_free_storage, NULL TSRMLS_CC);
	if(ce == pdo_dbh_ce) {
		retval.handlers = &pdo_dbh_object_handlers;
	}
	else {
		retval.handlers = &std_object_handlers;
	}
	return retval;
}

/* }}} */

ZEND_RSRC_DTOR_FUNC(php_pdo_pdbh_dtor)
{
	if (rsrc->ptr) {
		pdo_dbh_t *dbh = (pdo_dbh_t*)rsrc->ptr;
		dbh_free(dbh TSRMLS_CC);
		rsrc->ptr = NULL;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
