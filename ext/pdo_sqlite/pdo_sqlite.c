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
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "SAPI.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_sqlite.h"
#include "php_pdo_sqlite_int.h"
#include "zend_exceptions.h"
#include "pdo_sqlite_arginfo.h"

static zend_class_entry *pdosqlite_ce;

/* {{{ pdo_sqlite_deps */
static const zend_module_dep pdo_sqlite_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
/* }}} */

/* {{{ pdo_sqlite_module_entry */
zend_module_entry pdo_sqlite_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_sqlite_deps,
	"pdo_sqlite",
	NULL,
	PHP_MINIT(pdo_sqlite),
	PHP_MSHUTDOWN(pdo_sqlite),
	NULL,
	NULL,
	PHP_MINFO(pdo_sqlite),
	PHP_PDO_SQLITE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#if defined(COMPILE_DL_PDO_SQLITE) || defined(COMPILE_DL_PDO_SQLITE_EXTERNAL)
ZEND_GET_MODULE(pdo_sqlite)
#endif

/* proto bool PdoSqlite::createFunction(string $function_name, callable $callback, int $num_args = -1, int $flags = 0)
    Creates a function that can be used in a query
*/
PHP_METHOD(PdoSqlite, createFunction)
{
	pdo_sqlite_create_function_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

#ifndef PDO_SQLITE_OMIT_LOAD_EXTENSION
/* Attempts to load an SQLite extension library. */
PHP_METHOD(PdoSqlite, loadExtension)
{
	char *extension, *errtext = NULL;
	char fullpath[MAXPATHLEN];
	size_t extension_len;

	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *db_handle;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &extension, &extension_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (extension_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	db_handle = (pdo_sqlite_db_handle *)dbh->driver_data;

#ifdef ZTS
	if ((strncmp(sapi_module.name, "cgi", 3) != 0) &&
		(strcmp(sapi_module.name, "cli") != 0) &&
		(strncmp(sapi_module.name, "embed", 5) != 0)
	) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "Not supported in multithreaded Web servers");
		RETURN_THROWS();
	}
#endif

	if (!VCWD_REALPATH(extension, fullpath)) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "Unable to load extension \"%s\"", extension);
		RETURN_THROWS();
	}

	sqlite3 *sqlite_handle;
	sqlite_handle = db_handle->db;

	/* This only enables extension loading for the C api, not for SQL */
	sqlite3_db_config(sqlite_handle, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, NULL);

	if (sqlite3_load_extension(sqlite_handle, fullpath, 0, &errtext) != SQLITE_OK) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "Unable to load extension \"%s\"", errtext);
		sqlite3_free(errtext);
		sqlite3_db_config(sqlite_handle, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 0, NULL);
		RETURN_THROWS();
	}

	/* We disable extension loading for a vague feeling of safety. This is probably not necessary
	as extensions can only be loaded through C code, not through SQL, and if someone can get
	some C code to run on the server, they can do anything.*/
	sqlite3_db_config(sqlite_handle, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 0, NULL);
}
#endif

typedef struct {
	sqlite3_blob *blob;
	size_t		 position;
	size_t       size;
	int          flags;
} php_stream_pdosqlite3_data;

static ssize_t php_pdosqlite3_stream_write(php_stream *stream, const char *buf, size_t count)
{
	php_stream_pdosqlite3_data *sqlite3_stream = (php_stream_pdosqlite3_data *) stream->abstract;

	if (sqlite3_stream->flags & SQLITE_OPEN_READONLY) {
		php_error_docref(NULL, E_WARNING, "Can't write to blob stream: is open as read only");
		return -1;
	}

	if (sqlite3_stream->position + count > sqlite3_stream->size) {
		php_error_docref(NULL, E_WARNING, "It is not possible to increase the size of a BLOB");
		return -1;
	}

	if (sqlite3_blob_write(sqlite3_stream->blob, buf, count, sqlite3_stream->position) != SQLITE_OK) {
		return -1;
	}

	if (sqlite3_stream->position + count >= sqlite3_stream->size) {
		stream->eof = 1;
		sqlite3_stream->position = sqlite3_stream->size;
	}
	else {
		sqlite3_stream->position += count;
	}

	return count;
}

static ssize_t php_pdosqlite3_stream_read(php_stream *stream, char *buf, size_t count)
{
	php_stream_pdosqlite3_data *sqlite3_stream = (php_stream_pdosqlite3_data *) stream->abstract;

	if (sqlite3_stream->position + count >= sqlite3_stream->size) {
		count = sqlite3_stream->size - sqlite3_stream->position;
		stream->eof = 1;
	}
	if (count) {
		if (sqlite3_blob_read(sqlite3_stream->blob, buf, count, sqlite3_stream->position) != SQLITE_OK) {
			return -1;
		}
		sqlite3_stream->position += count;
	}
	return count;
}

static int php_pdosqlite3_stream_close(php_stream *stream, int close_handle)
{
	php_stream_pdosqlite3_data *sqlite3_stream = (php_stream_pdosqlite3_data *) stream->abstract;

	if (sqlite3_blob_close(sqlite3_stream->blob) != SQLITE_OK) {
		/* Error occurred, but it still closed */
	}

	efree(sqlite3_stream);

	return 0;
}

static int php_pdosqlite3_stream_flush(php_stream *stream)
{
	/* do nothing */
	return 0;
}

static int php_pdosqlite3_stream_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffs)
{
	php_stream_pdosqlite3_data *sqlite3_stream = (php_stream_pdosqlite3_data *) stream->abstract;

	switch(whence) {
		case SEEK_CUR:
			if (offset < 0) {
				if (sqlite3_stream->position < (size_t)(-offset)) {
					sqlite3_stream->position = 0;
					*newoffs = -1;
					return -1;
				} else {
					sqlite3_stream->position = sqlite3_stream->position + offset;
					*newoffs = sqlite3_stream->position;
					stream->eof = 0;
					return 0;
				}
			} else {
				if (sqlite3_stream->position + (size_t)(offset) > sqlite3_stream->size) {
					sqlite3_stream->position = sqlite3_stream->size;
					*newoffs = -1;
					return -1;
				} else {
					sqlite3_stream->position = sqlite3_stream->position + offset;
					*newoffs = sqlite3_stream->position;
					stream->eof = 0;
					return 0;
				}
			}
		case SEEK_SET:
			if (sqlite3_stream->size < (size_t)(offset)) {
				sqlite3_stream->position = sqlite3_stream->size;
				*newoffs = -1;
				return -1;
			} else {
				sqlite3_stream->position = offset;
				*newoffs = sqlite3_stream->position;
				stream->eof = 0;
				return 0;
			}
		case SEEK_END:
			if (offset > 0) {
				sqlite3_stream->position = sqlite3_stream->size;
				*newoffs = -1;
				return -1;
			} else if (sqlite3_stream->size < (size_t)(-offset)) {
				sqlite3_stream->position = 0;
				*newoffs = -1;
				return -1;
			} else {
				sqlite3_stream->position = sqlite3_stream->size + offset;
				*newoffs = sqlite3_stream->position;
				stream->eof = 0;
				return 0;
			}
		default:
			*newoffs = sqlite3_stream->position;
			return -1;
	}
}

static int php_pdosqlite3_stream_cast(php_stream *stream, int castas, void **ret)
{
	return FAILURE;
}

static int php_pdosqlite3_stream_stat(php_stream *stream, php_stream_statbuf *ssb)
{
	php_stream_pdosqlite3_data *sqlite3_stream = (php_stream_pdosqlite3_data *) stream->abstract;
	ssb->sb.st_size = sqlite3_stream->size;
	return 0;
}

static const php_stream_ops php_stream_pdosqlite3_ops = {
		php_pdosqlite3_stream_write,
		php_pdosqlite3_stream_read,
		php_pdosqlite3_stream_close,
		php_pdosqlite3_stream_flush,
		"PDOSQLite",
		php_pdosqlite3_stream_seek,
		php_pdosqlite3_stream_cast,
		php_pdosqlite3_stream_stat,
		NULL
};

/* Open a blob as a stream which we can read / write to. */
PHP_METHOD(PdoSqlite, openBlob)
{
	char *table, *column, *dbname = "main", *mode = "rb";
	size_t table_len, column_len, dbname_len;
	zend_long rowid, flags = SQLITE_OPEN_READONLY, sqlite_flags = 0;
	sqlite3_blob *blob = NULL;
	php_stream_pdosqlite3_data *sqlite3_stream;
	php_stream *stream;

	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *db_handle;

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	db_handle = (pdo_sqlite_db_handle *)dbh->driver_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ppl|pl", &table, &table_len, &column, &column_len, &rowid, &dbname, &dbname_len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	sqlite3 *sqlite_handle;
	sqlite_handle = db_handle->db;
	sqlite_flags = (flags & SQLITE_OPEN_READWRITE) ? 1 : 0;

	if (sqlite3_blob_open(sqlite_handle, dbname, table, column, rowid, sqlite_flags, &blob) != SQLITE_OK) {
		zend_error(E_WARNING, "Unable to open blob: %s", sqlite3_errmsg(sqlite_handle));
		RETURN_FALSE;
	}

	sqlite3_stream = emalloc(sizeof(php_stream_pdosqlite3_data));
	sqlite3_stream->blob = blob;
	sqlite3_stream->flags = flags;
	sqlite3_stream->position = 0;
	sqlite3_stream->size = sqlite3_blob_bytes(blob);

	if (sqlite_flags != 0) {
		mode = "r+b";
	}

	stream = php_stream_alloc(&php_stream_pdosqlite3_ops, sqlite3_stream, 0, mode);

	if (stream) {
		php_stream_to_zval(stream, return_value);
	} else {
		RETURN_FALSE;
	}
}

static int php_sqlite_collation_callback(void *context, int string1_len, const void *string1,
	int string2_len, const void *string2)
{
	int ret;
	zval zargs[2];
	zval retval;
	struct pdo_sqlite_collation *collation = (struct pdo_sqlite_collation*) context;

	collation->fc.fci.size = sizeof(collation->fc.fci);
	ZVAL_COPY_VALUE(&collation->fc.fci.function_name, &collation->callback);
	collation->fc.fci.object = NULL;
	collation->fc.fci.retval = &retval;

	// Prepare the arguments.
	ZVAL_STRINGL(&zargs[0], (char *) string1, string1_len);
	ZVAL_STRINGL(&zargs[1], (char *) string2, string2_len);
	collation->fc.fci.param_count = 2;
	collation->fc.fci.params = zargs;

	if ((ret = zend_call_function(&collation->fc.fci, &collation->fc.fcc)) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "An error occurred while invoking the callback");
	} else if (!Z_ISUNDEF(retval)) {
		if (Z_TYPE(retval) != IS_LONG) {
			zend_string *func_name = get_active_function_or_method_name();
			zend_type_error("%s(): Return value of the callback must be of type int, %s returned",
				ZSTR_VAL(func_name), zend_zval_value_name(&retval));
			zend_string_release(func_name);
			return FAILURE;
		}
		ret = 0;
		if (Z_LVAL(retval) > 0) {
			ret = 1;
		} else if (Z_LVAL(retval) < 0) {
			ret = -1;
		}
		zval_ptr_dtor(&retval);
	}

	zval_ptr_dtor(&zargs[0]);
	zval_ptr_dtor(&zargs[1]);

	return ret;
}

PHP_METHOD(PdoSqlite, createAggregate)
{
	pdo_sqlite_create_aggregate_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(PdoSqlite, createCollation)
{
	pdo_sqlite_create_collation_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_sqlite_collation_callback);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo_sqlite)
{
#ifdef SQLITE_DETERMINISTIC
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_DETERMINISTIC", (zend_long)SQLITE_DETERMINISTIC);
#endif

	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_ATTR_OPEN_FLAGS", (zend_long)PDO_SQLITE_ATTR_OPEN_FLAGS);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_OPEN_READONLY", (zend_long)SQLITE_OPEN_READONLY);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_OPEN_READWRITE", (zend_long)SQLITE_OPEN_READWRITE);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_OPEN_CREATE", (zend_long)SQLITE_OPEN_CREATE);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_ATTR_READONLY_STATEMENT", (zend_long)PDO_SQLITE_ATTR_READONLY_STATEMENT);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_ATTR_EXTENDED_RESULT_CODES", (zend_long)PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES);

	pdosqlite_ce = register_class_PdoSqlite(pdo_dbh_ce);
	pdosqlite_ce->create_object = pdo_dbh_new;

	if (php_pdo_register_driver(&pdo_sqlite_driver) == FAILURE) {
		return FAILURE;
	}

	return php_pdo_register_driver_specific_ce(&pdo_sqlite_driver, pdosqlite_ce);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pdo_sqlite)
{
	php_pdo_unregister_driver(&pdo_sqlite_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdo_sqlite)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "PDO Driver for SQLite 3.x", "enabled");
	php_info_print_table_row(2, "SQLite Library", sqlite3_libversion());
	php_info_print_table_end();
}
/* }}} */
