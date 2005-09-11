/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
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

void pdo_raise_impl_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *sqlstate, const char *supp TSRMLS_DC)
{
	pdo_error_type *pdo_err = &dbh->error_code;
	char *message = NULL;
	const char *msg;
	zval *info = NULL;

	if (dbh->error_mode == PDO_ERRMODE_SILENT) {
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

	strcpy(*pdo_err, sqlstate);

	/* hash sqlstate to error messages */
	msg = pdo_sqlstate_state_to_description(*pdo_err);
	if (!msg) {
		msg = "<<Unknown error>>";
	}

	MAKE_STD_ZVAL(info);
	array_init(info);

	add_next_index_string(info, *pdo_err, 1);
	add_next_index_long(info, 0);
		
	if (supp) {
		spprintf(&message, 0, "SQLSTATE[%s]: %s: %s", *pdo_err, msg, supp);
	} else {
		spprintf(&message, 0, "SQLSTATE[%s]: %s", *pdo_err, msg);
	}

	if (dbh->error_mode != PDO_ERRMODE_EXCEPTION) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", message);

		if (info) {
			zval_ptr_dtor(&info);
		}
	} else {
		zval *ex;
		zend_class_entry *def_ex = zend_exception_get_default(TSRMLS_C), *pdo_ex = php_pdo_get_exception(TSRMLS_C);

		MAKE_STD_ZVAL(ex);
		object_init_ex(ex, pdo_ex);

		zend_update_property_string(def_ex, ex, "message", sizeof("message")-1, message TSRMLS_CC);
		zend_update_property_string(def_ex, ex, "code", sizeof("code")-1, *pdo_err TSRMLS_CC);
		
		if (info) {
			zend_update_property(pdo_ex, ex, "errorInfo", sizeof("errorInfo")-1, info TSRMLS_CC);
			zval_ptr_dtor(&info);
		}

		zend_throw_exception_object(ex TSRMLS_CC);
	}
	
	if (message) {
		efree(message);
	}
}

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

		zval_ptr_dtor(&info);
		info = NULL;
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
	} else if (EG(exception) == NULL) {
		zval *ex;
		zend_class_entry *def_ex = zend_exception_get_default(TSRMLS_C), *pdo_ex = php_pdo_get_exception(TSRMLS_C);

		MAKE_STD_ZVAL(ex);
		object_init_ex(ex, pdo_ex);

		zend_update_property_string(def_ex, ex, "message", sizeof("message")-1, message TSRMLS_CC);
		zend_update_property_string(def_ex, ex, "code", sizeof("code")-1, *pdo_err TSRMLS_CC);
		
		if (info) {
			zend_update_property(pdo_ex, ex, "errorInfo", sizeof("errorInfo")-1, info TSRMLS_CC);
			zval_ptr_dtor(&info);
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

/* {{{ proto object PDO::__construct(string dsn, string username, string passwd [, array options])
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
	zval *options = NULL;
	char alt_dsn[512];
	int call_factory = 1;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!s!a!", &data_source, &data_source_len,
				&username, &usernamelen, &password, &passwordlen, &options)) {
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
			zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "invalid data source name");
			ZVAL_NULL(object);
			return;
		}

		data_source = ini_dsn;
		colon = strchr(data_source, ':');
		
		if (!colon) {
			zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "invalid data source name (via INI: %s)", alt_dsn);
			ZVAL_NULL(object);
			return;
		}
	}

	if (!strncmp(data_source, "uri:", sizeof("uri:")-1)) {
		/* the specified URI holds connection details */
		data_source = dsn_from_uri(data_source, alt_dsn, sizeof(alt_dsn) TSRMLS_CC);
		if (!data_source) {
			zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "invalid data source URI");
			ZVAL_NULL(object);
			return;
		}
		colon = strchr(data_source, ':');
		if (!colon) {
			zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "invalid data source name (via URI)");
			ZVAL_NULL(object);
			return;
		}
	}

	driver = pdo_find_driver(data_source, colon - data_source);

	if (!driver) {
		/* NB: don't want to include the data_source in the error message as
		 * it might contain a password */
		zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "could not find driver");
		ZVAL_NULL(object);
		return;
	}
	
	dbh = (pdo_dbh_t *) zend_object_store_get_object(object TSRMLS_CC);

	/* is this supposed to be a persistent connection ? */
	if (options) {
		zval **v;
		int plen;
		char *hashkey = NULL;
		list_entry *le;
		pdo_dbh_t *pdbh = NULL;

		if (SUCCESS == zend_hash_index_find(Z_ARRVAL_P(options), PDO_ATTR_PERSISTENT, (void**)&v)) {
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

		if (is_persistent) {
			/* let's see if we have one cached.... */
			if (SUCCESS == zend_hash_find(&EG(persistent_list), hashkey, plen+1, (void*)&le)) {
				if (Z_TYPE_P(le) == php_pdo_list_entry()) {
					pdbh = (pdo_dbh_t*)le->ptr;

					/* is the connection still alive ? */
					if (pdbh->methods->check_liveness && FAILURE == (pdbh->methods->check_liveness)(pdbh TSRMLS_CC)) {
						/* nope... need to kill it */
						pdbh = NULL;
					}
				}
			}

			if (pdbh) {
				call_factory = 0;
			} else {
				/* need a brand new pdbh */
				pdbh = pecalloc(1, sizeof(*pdbh), 1);

				if (!pdbh) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "out of memory while allocating PDO handle");
					/* NOTREACHED */
				}

				pdbh->is_persistent = 1;
				if (!(pdbh->persistent_id = pemalloc(plen + 1, 1))) {
					php_error_docref(NULL TSRMLS_CC, E_ERROR, "out of memory while allocating PDO handle");
				}
				memcpy((char *)pdbh->persistent_id, hashkey, plen+1);
				pdbh->persistent_id_len = plen+1;
				pdbh->refcount = 1;
			}
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

	dbh->auto_commit = pdo_attr_lval(options, PDO_ATTR_AUTOCOMMIT, 1 TSRMLS_CC);

	if (!dbh->data_source || (username && !dbh->username) || (password && !dbh->password)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "out of memory");
	}

	if (!call_factory) {
		/* we got a persistent guy from our cache */
		return;
	}

	if (driver->db_handle_factory(dbh, options TSRMLS_CC)) {
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

		dbh->driver = driver;
		return;	
	}

	/* the connection failed; things will tidy up in free_storage */
	/* XXX raise exception */
	ZVAL_NULL(object);
}
/* }}} */

static zval *pdo_stmt_instantiate(pdo_dbh_t *dbh, zval *object, zend_class_entry *dbstmt_ce, zval *ctor_args TSRMLS_DC) /* {{{ */
{
	if (ctor_args) {
		if (Z_TYPE_P(ctor_args) != IS_ARRAY) {
			pdo_raise_impl_error(dbh, NULL, "HY000", "constructor arguments must be passed as an array" TSRMLS_CC);
			return NULL;
		}
		if (!dbstmt_ce->constructor) {
			pdo_raise_impl_error(dbh, NULL, "HY000", "user-supplied statement does not accept constructor arguments" TSRMLS_CC);
			return NULL;
		}
	}

	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, dbstmt_ce);
	object->refcount = 1;
	object->is_ref = 1;
	
	return object;
} /* }}} */

static void pdo_stmt_construct(pdo_stmt_t *stmt, zval *object, zend_class_entry *dbstmt_ce, zval *ctor_args TSRMLS_DC) /* {{{ */
{	
	zval *query_string;
	zval z_key;

	MAKE_STD_ZVAL(query_string);
	ZVAL_RT_STRINGL(query_string, stmt->query_string, stmt->query_stringlen, 1);
	ZVAL_ASCII_STRINGL(&z_key, "queryString", sizeof("queryString")-1, 0);
	std_object_handlers.write_property(object, &z_key, query_string TSRMLS_CC);
	zval_ptr_dtor(&query_string);
#ifdef UG
	if (UG(unicode)) {
		zval_dtor(&z_key);
	}
#endif

	if (dbstmt_ce->constructor) {
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval *retval;

		fci.size = sizeof(zend_fcall_info);
		fci.function_table = &dbstmt_ce->function_table;
		fci.function_name = NULL;
		fci.object_pp = &object;
		fci.symbol_table = NULL;
		fci.retval_ptr_ptr = &retval;
		if (ctor_args) {
			HashTable *ht = Z_ARRVAL_P(ctor_args);
			Bucket *p;

			fci.param_count = 0;
			fci.params = safe_emalloc(sizeof(zval*), ht->nNumOfElements, 0);
			p = ht->pListHead;
			while (p != NULL) {
				fci.params[fci.param_count++] = (zval**)p->pData;
				p = p->pListNext;
			}
		} else {
			fci.param_count = 0;
			fci.params = NULL;
		}
		fci.no_separation = 1;

		fcc.initialized = 1;
		fcc.function_handler = dbstmt_ce->constructor;
		fcc.calling_scope = EG(scope);
		fcc.object_pp = &object;

		if (zend_call_function(&fci, &fcc TSRMLS_CC) == FAILURE) {
			zval_dtor(object);
			ZVAL_NULL(object);
			object = NULL; /* marks failure */
		} else {
			zval_ptr_dtor(&retval);
		}
			
		if (fci.params) {
			efree(fci.params);
		}
	}
}
/* }}} */

/* {{{ proto object PDO::prepare(string statment [, array options])
   Prepares a statement for execution and returns a statement object */
static PHP_METHOD(PDO, prepare)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	pdo_stmt_t *stmt;
	char *statement;
	int statement_len;
	zval *options = NULL, **opt, **item, *ctor_args;
	zend_class_entry *dbstmt_ce, **pce;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a", &statement,
			&statement_len, &options)) {
		RETURN_FALSE;
	}
	
	PDO_DBH_CLEAR_ERR();

	if (ZEND_NUM_ARGS() > 1 && SUCCESS == zend_hash_index_find(Z_ARRVAL_P(options), PDO_ATTR_STATEMENT_CLASS, (void**)&opt)) {
		if (zend_hash_index_find(Z_ARRVAL_PP(opt), 0, (void**)&item) == FAILURE
			|| !PDO_ZVAL_PP_IS_TEXT(item)
			|| zend_u_lookup_class(Z_TYPE_PP(item), Z_UNIVAL_PP(item), Z_UNILEN_PP(item), &pce TSRMLS_CC) == FAILURE
		) {
			pdo_raise_impl_error(dbh, NULL, "HY000", 
				"PDO_ATTR_STATEMENT_CLASS requires format array(classname, ctor_args); "
				"the classname must be a string specifying an existing class"
				TSRMLS_CC);
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		}
		dbstmt_ce = *pce;
		if (!instanceof_function(dbstmt_ce, U_CLASS_ENTRY(pdo_dbstmt_ce) TSRMLS_CC)) {
			pdo_raise_impl_error(dbh, NULL, "HY000", 
				"user-supplied statement class must be derived from PDOStatement" TSRMLS_CC);
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		}
		if (dbstmt_ce->constructor && !(dbstmt_ce->constructor->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED))) {
			pdo_raise_impl_error(dbh, NULL, "HY000", 
				"user-supplied statement class cannot have a public constructor" TSRMLS_CC);
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		}
		if (zend_hash_index_find(Z_ARRVAL_PP(opt), 1, (void**)&item) == SUCCESS) {
			if (Z_TYPE_PP(item) != IS_ARRAY) {
				pdo_raise_impl_error(dbh, NULL, "HY000", 
					"PDO_ATTR_STATEMENT_CLASS requires format array(classname, ctor_args); "
					"ctor_args must be an array"
				TSRMLS_CC);
				PDO_HANDLE_DBH_ERR();
				RETURN_FALSE;
			}
			ctor_args = *item;
		} else {
			ctor_args = NULL;
		}
	} else {
		dbstmt_ce = U_CLASS_ENTRY(pdo_dbstmt_ce);
		ctor_args = NULL;
	}

	if (!pdo_stmt_instantiate(dbh, return_value, dbstmt_ce, ctor_args TSRMLS_CC)) {
		pdo_raise_impl_error(dbh, NULL, "HY000", 
			"failed to instantiate user-supplied statement class"
			TSRMLS_CC);
		PDO_HANDLE_DBH_ERR();
		RETURN_FALSE;
	}
	stmt = (pdo_stmt_t*)zend_object_store_get_object(return_value TSRMLS_CC);
	
	/* unconditionally keep this for later reference */
	stmt->query_string = estrndup(statement, statement_len);
	stmt->query_stringlen = statement_len;
	stmt->default_fetch_type = PDO_FETCH_BOTH;
	stmt->dbh = dbh;
	/* give it a reference to me */
	zend_objects_store_add_ref(getThis() TSRMLS_CC);
	stmt->database_object_handle = *getThis();
	/* we haven't created a lazy object yet */
	ZVAL_NULL(&stmt->lazy_object_ref);

	if (dbh->methods->preparer(dbh, statement, statement_len, stmt, options TSRMLS_CC)) {
		pdo_stmt_construct(stmt, return_value, dbstmt_ce, ctor_args TSRMLS_CC);
		return;
	}

	PDO_HANDLE_DBH_ERR();

	/* kill the object handle for the stmt here */
	zval_dtor(return_value);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool PDO::beginTransaction()
   Initiates a transaction */
static PHP_METHOD(PDO, beginTransaction)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (dbh->in_txn) {
		zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "There is already an active transaction");
		RETURN_FALSE;
	}
	
	if (!dbh->methods->begin) {
		/* TODO: this should be an exception; see the auto-commit mode
		 * comments below */
		zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "This driver doesn't support transactions");
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
		zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "There is no active transaction");
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
		zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "There is no active transaction");
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
					pdo_raise_impl_error(dbh, NULL, "HY000", "invalid error mode" TSRMLS_CC);
					PDO_HANDLE_DBH_ERR();
					RETURN_FALSE;
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
					pdo_raise_impl_error(dbh, NULL, "HY000", "invalid case folding mode" TSRMLS_CC);
					PDO_HANDLE_DBH_ERR();
					RETURN_FALSE;
			}
			RETURN_FALSE;

		case PDO_ATTR_ORACLE_NULLS:
			convert_to_long(value);
			dbh->oracle_nulls = Z_LVAL_P(value);
			RETURN_TRUE;

		case PDO_ATTR_STRINGIFY_FETCHES:
			convert_to_long(value);
			dbh->stringify = Z_LVAL_P(value) ? 1 : 0;
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
		zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "The auto-commit mode cannot be changed for this driver");
	} else if (!dbh->methods->set_attribute) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support setting attributes" TSRMLS_CC);
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

	PDO_DBH_CLEAR_ERR();

	/* handle generic PDO-level atributes */
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
			RETURN_STRINGL(dbh->driver->driver_name, dbh->driver->driver_name_len, 1);
	}
	
	if (!dbh->methods->get_attribute) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support getting attributes" TSRMLS_CC);
		RETURN_FALSE;
	}

	switch (dbh->methods->get_attribute(dbh, attr, return_value TSRMLS_CC)) {
		case -1:
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;

		case 0:
			pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support that attribute" TSRMLS_CC);
			RETURN_FALSE;

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


/* {{{ proto string PDO::lastInsertId([string seqname])
   Returns the id of the last row that we affected on this connection.  Some databases require a sequence or table name to be passed in.  Not always meaningful. */
static PHP_METHOD(PDO, lastInsertId)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	char *name = NULL;
	int namelen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!", &name, &namelen)) {
		RETURN_FALSE;
	}

	PDO_DBH_CLEAR_ERR();
	if (!dbh->methods->last_id) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support lastInsertId()" TSRMLS_CC);
		RETURN_FALSE;
	} else {
		Z_STRVAL_P(return_value) = dbh->methods->last_id(dbh, name, &Z_STRLEN_P(return_value) TSRMLS_CC);
		if (!Z_STRVAL_P(return_value)) {
			PDO_HANDLE_DBH_ERR();
			RETURN_FALSE;
		} else {
			Z_TYPE_P(return_value) = IS_STRING;
		}
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

	if (FAILURE == zend_parse_parameters(1 TSRMLS_CC, "s", &statement,
			&statement_len)) {
		RETURN_FALSE;
	}
	
	PDO_DBH_CLEAR_ERR();

	if (!pdo_stmt_instantiate(dbh, return_value, U_CLASS_ENTRY(pdo_dbstmt_ce), NULL TSRMLS_CC)) {
		pdo_raise_impl_error(dbh, NULL, "HY000", "failed to instantiate user supplied statement class" TSRMLS_CC);
		return;
	}
	stmt = (pdo_stmt_t*)zend_object_store_get_object(return_value TSRMLS_CC);
	
	/* unconditionally keep this for later reference */
	stmt->query_string = estrndup(statement, statement_len);
	stmt->query_stringlen = statement_len;
	stmt->default_fetch_type = PDO_FETCH_BOTH;
	stmt->active_query_string = stmt->query_string;
	stmt->active_query_stringlen = statement_len;
	stmt->dbh = dbh;
	/* give it a reference to me */
	zend_objects_store_add_ref(getThis() TSRMLS_CC);
	stmt->database_object_handle = *getThis();
	/* we haven't created a lazy object yet */
	ZVAL_NULL(&stmt->lazy_object_ref);

	if (dbh->methods->preparer(dbh, statement, statement_len, stmt, NULL TSRMLS_CC)) {
		if (ZEND_NUM_ARGS() == 1 || SUCCESS == pdo_stmt_setup_fetch_mode(INTERNAL_FUNCTION_PARAM_PASSTHRU, stmt, 1)) {
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
					pdo_stmt_construct(stmt, return_value, U_CLASS_ENTRY(pdo_dbstmt_ce), NULL TSRMLS_CC);
					return;
				}
			}
		}
		/* something broke */
	}

	PDO_HANDLE_STMT_ERR();
		
	/* kill the object handle for the stmt here */
	zval_dtor(return_value);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string PDO::quote(string string [, int paramtype])
   quotes string for use in a query.  The optional paramtype acts as a hint for drivers that have alternate quoting styles.  The default value is PDO_PARAM_STR */
static PHP_METHOD(PDO, quote)
{
	pdo_dbh_t *dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	char *str;
	int str_len;
	long paramtype = PDO_PARAM_STR;
	char *qstr;
	int qlen;

	if (FAILURE == zend_parse_parameters(1 TSRMLS_CC, "s|l", &str, &str_len,
			&paramtype)) {
		RETURN_FALSE;
	}
	
	PDO_DBH_CLEAR_ERR();
	if (!dbh->methods->quoter) {
		pdo_raise_impl_error(dbh, NULL, "IM001", "driver does not support quoting" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (dbh->methods->quoter(dbh, str, str_len, &qstr, &qlen, paramtype TSRMLS_CC)) {
		RETURN_STRINGL(qstr, qlen, 0);
	}
	PDO_HANDLE_DBH_ERR();
}
/* }}} */

/* {{{ proto int PDO::__wakeup()
   Prevents use of a PDO instance that has been unserialized */
static PHP_METHOD(PDO, __wakeup)
{
	zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "You cannot serialize or unserialize PDO instances");
}
/* }}} */

/* {{{ proto int PDO::__sleep()
   Prevents serialization of a PDO instance */
static PHP_METHOD(PDO, __sleep)
{
	zend_throw_exception_ex(php_pdo_get_exception(TSRMLS_C), 0 TSRMLS_CC, "You cannot serialize or unserialize PDO instances");
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
	PHP_ME(PDO, __wakeup,		NULL,					ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	PHP_ME(PDO, __sleep,		NULL,					ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	PHP_ME(PDO, quote,			NULL,					ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ overloaded object handlers for PDO class */
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
			PDO_DBH_DRIVER_METHOD_KIND_DBH TSRMLS_CC);
	if (!funcs) {
		return 0;
	}

	if (!(dbh->cls_methods[kind] = pemalloc(sizeof(HashTable), dbh->is_persistent))) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "out of memory while allocating PDO methods.");
	}
	zend_hash_init_ex(dbh->cls_methods[kind], 8, NULL, NULL, dbh->is_persistent, 0);

	while (funcs->fname) {
		ifunc->type = ZEND_INTERNAL_FUNCTION;
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
#ifdef IS_UNICODE
	zend_uchar ztype = UG(unicode) ? IS_UNICODE : IS_STRING;
#endif

	lc_method_name = zend_u_str_tolower_dup(ztype, method_name, method_len);

	if (zend_u_hash_find(&dbh->ce->function_table, ztype, lc_method_name, method_len+1, (void**)&fbc) == FAILURE) {
		/* not a pre-defined method, nor a user-defined method; check
		 * the driver specific methods */
		if (!dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH]) {
			if (!pdo_hash_methods(dbh,
				PDO_DBH_DRIVER_METHOD_KIND_DBH TSRMLS_CC)
				|| !dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH]) {
				goto out;
			}
		}

		if (zend_u_hash_find(dbh->cls_methods[PDO_DBH_DRIVER_METHOD_KIND_DBH],
				ztype, lc_method_name, method_len+1, (void**)&fbc) == FAILURE) {
			fbc = NULL;
			goto out;
		}
		/* got it */
	}
	
out:
	efree(lc_method_name);
	return fbc;
}

static int dbh_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	return -1;
}

static zend_object_handlers pdo_dbh_object_handlers;

void pdo_dbh_init(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PDO", pdo_dbh_functions);
	pdo_dbh_ce = zend_register_internal_class(&ce TSRMLS_CC);
	pdo_dbh_ce->create_object = pdo_dbh_new;
	pdo_dbh_ce->constructor->common.fn_flags |= ZEND_ACC_FINAL;

	memcpy(&pdo_dbh_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_dbh_object_handlers.get_method = dbh_method_get;
	pdo_dbh_object_handlers.compare_objects = dbh_compare;
}

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

static void pdo_dbh_free_storage(pdo_dbh_t *dbh TSRMLS_DC)
{
	if (dbh->in_txn && dbh->methods && dbh->methods->rollback) {
		dbh->methods->rollback(dbh TSRMLS_CC);
		dbh->in_txn = 0;
	}
	
	if (dbh->properties) {
		zend_hash_destroy(dbh->properties);
		efree(dbh->properties);
		dbh->properties = NULL;
	}

	if (!dbh->is_persistent) {
		dbh_free(dbh TSRMLS_CC);
	} else if (dbh->methods->persistent_shutdown) {
		dbh->methods->persistent_shutdown(dbh TSRMLS_CC);
	}
}

zend_object_value pdo_dbh_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	pdo_dbh_t *dbh;
	zval *tmp;

	dbh = emalloc(sizeof(*dbh));
	memset(dbh, 0, sizeof(*dbh));
	dbh->ce = ce;
	dbh->refcount = 1;
	ALLOC_HASHTABLE(dbh->properties);
	zend_hash_init(dbh->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(dbh->properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(dbh, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)pdo_dbh_free_storage, NULL TSRMLS_CC);
	retval.handlers = &pdo_dbh_object_handlers;
	
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
