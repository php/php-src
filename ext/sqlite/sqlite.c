/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_sqlite.h"

#include <sqlite.h>

static unsigned char arg3_force_ref[] = {3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

static int le_sqlite_db, le_sqlite_result;

struct php_sqlite_result {
	char **table;
	int nrows;
	int ncolumns;
	int curr_row;
};

enum { PHPSQLITE_ASSOC = 1, PHPSQLITE_NUM = 2, PHPSQLITE_BOTH = PHPSQLITE_ASSOC|PHPSQLITE_NUM };

function_entry sqlite_functions[] = {
	PHP_FE(sqlite_open, arg3_force_ref)
	PHP_FE(sqlite_close, NULL)
	PHP_FE(sqlite_query, NULL)
	PHP_FE(sqlite_fetch_array, NULL)
	PHP_FE(sqlite_libversion, NULL)
	PHP_FE(sqlite_libencoding, NULL)
	PHP_FE(sqlite_changes, NULL)
	PHP_FE(sqlite_last_insert_rowid, NULL)
	PHP_FE(sqlite_num_rows, NULL)
	PHP_FE(sqlite_num_fields, NULL)
	PHP_FE(sqlite_field_name, NULL)
	PHP_FE(sqlite_seek, NULL)
	{NULL, NULL, NULL}
};


zend_module_entry sqlite_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"sqlite",
	sqlite_functions,
	PHP_MINIT(sqlite),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(sqlite),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1",
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_SQLITE
ZEND_GET_MODULE(sqlite)
#endif


static ZEND_RSRC_DTOR_FUNC(php_sqlite_db_dtor)
{
	sqlite *db = (sqlite*)rsrc->ptr;
	
	sqlite_close(db);
}

static ZEND_RSRC_DTOR_FUNC(php_sqlite_result_dtor)
{
	struct php_sqlite_result *res = (struct php_sqlite_result *)rsrc->ptr;

	sqlite_free_table(res->table);
	efree(res);
}

PHP_MINIT_FUNCTION(sqlite)
{
	le_sqlite_db = zend_register_list_destructors_ex(php_sqlite_db_dtor, NULL, "sqlite database", module_number);
	le_sqlite_result = zend_register_list_destructors_ex(php_sqlite_result_dtor, NULL, "sqlite result", module_number);

	REGISTER_LONG_CONSTANT("SQLITE_BOTH",	PHPSQLITE_BOTH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_NUM",	PHPSQLITE_NUM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_ASSOC",	PHPSQLITE_ASSOC, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MINFO_FUNCTION(sqlite)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "SQLite support", "enabled");
	php_info_print_table_row(2, "PECL Module version", "$Id$");
	php_info_print_table_row(2, "SQLite Library", sqlite_libversion());
	php_info_print_table_row(2, "SQLite Encoding", sqlite_libencoding());
	php_info_print_table_end();
}

/* {{{ proto resource sqlite_open(string filename [, int mode, string &errmessage])
   Opens an SQLite database.  Will create the database if it does not exist */
PHP_FUNCTION(sqlite_open)
{
	int mode = 0666;
	char *filename;
	long filename_len;
	zval *errmsg = NULL;
	sqlite *db = NULL;
	char *errtext = NULL;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz/",
				&filename, &filename_len, &mode, &errmsg)) {
		return;
	}

	/* TODO: safemode and open_basedir checks on the filename */
	
	db = sqlite_open(filename, mode, &errtext);

	if (db == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);

		if (errmsg) {
			ZVAL_STRING(errmsg, errtext, 1);
		}

		sqlite_freemem(errtext);

		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, db, le_sqlite_db);
	
}
/* }}} */

/* {{{ proto void sqlite_close(resource db)
   Closes an open sqlite database */
PHP_FUNCTION(sqlite_close)
{
	zval *zdb;
	sqlite *db;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}

	ZEND_FETCH_RESOURCE(db, sqlite *, &zdb, -1, "sqlite database", le_sqlite_db);
	zend_list_delete(Z_RESVAL_P(zdb));
}
/* }}} */

/* {{{ proto resource sqlite_query(string query, resource db)
   Executes a query against a given database and returns a result handle */
PHP_FUNCTION(sqlite_query)
{
	zval *zdb;
	sqlite *db;
	char *sql;
	long sql_len;
	struct php_sqlite_result res, *rres;
	int ret;
	char *errtext;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &sql, &sql_len, &zdb)) {
		return;
	}

	ZEND_FETCH_RESOURCE(db, sqlite *, &zdb, -1, "sqlite database", le_sqlite_db);

	memset(&res, 0, sizeof(res));

	ret = sqlite_get_table(db, sql, &res.table, &res.nrows, &res.ncolumns, &errtext);

	if (ret != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
		sqlite_freemem(errtext);
		RETURN_FALSE;
	}

	rres = (struct php_sqlite_result*)emalloc(sizeof(*rres));
	memcpy(rres, &res, sizeof(*rres));

	rres->curr_row = 1;	/* 0 holds the column header names */

	ZEND_REGISTER_RESOURCE(return_value, rres, le_sqlite_result);
}
/* }}} */

/* {{{ proto array sqlite_fetch_array(resource result [, int result_type])
   Fetches the next row from a result set as an array */
PHP_FUNCTION(sqlite_fetch_array)
{
	zval *zres;
	struct php_sqlite_result *res;
	int mode = PHPSQLITE_BOTH;
	int i, j;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zres, &mode)) {
		return;
	}

	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);
	
	/* check range of the row */
	if (res->curr_row > res->nrows) {
		/* no more */
		RETURN_FALSE;
	}

	array_init(return_value);
	
	/* calculate the starting slot for the row */
	i = res->curr_row * res->ncolumns;

	/* now populate the result */
	for (j = 0; j < res->ncolumns; j++) {
		if (mode & PHPSQLITE_NUM) {
			add_index_string(return_value, j, res->table[i + j], 1);
		}
		if (mode & PHPSQLITE_ASSOC) {
			add_assoc_string(return_value, res->table[j], res->table[i + j], 1);
		}
	}

	/* advance the row pointer */
	res->curr_row++;
}
/* }}} */

/* {{{ proto string sqlite_libversion()
   Returns the version of the linked SQLite library */
PHP_FUNCTION(sqlite_libversion)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_STRING((char*)sqlite_libversion(), 1);
}
/* }}} */

/* {{{ proto string sqlite_libencoding()
   Returns the encoding (iso8859 or UTF-8) of the linked SQLite library */
PHP_FUNCTION(sqlite_libencoding)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	RETURN_STRING((char*)sqlite_libencoding(), 1);
}
/* }}} */

/* {{{ proto int sqlite_changes(resource db)
   Returns the number of rows that were changed by the most recent SQL statement */
PHP_FUNCTION(sqlite_changes)
{
	zval *zdb;
	sqlite *db;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}

	ZEND_FETCH_RESOURCE(db, sqlite *, &zdb, -1, "sqlite database", le_sqlite_db);

	RETURN_LONG(sqlite_changes(db));
}
/* }}} */

/* {{{ proto int sqlite_last_insert_rowid(resource db)
   Returns the rowid of the most recently inserted row */
PHP_FUNCTION(sqlite_last_insert_rowid)
{
	zval *zdb;
	sqlite *db;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}

	ZEND_FETCH_RESOURCE(db, sqlite *, &zdb, -1, "sqlite database", le_sqlite_db);

	RETURN_LONG(sqlite_last_insert_rowid(db));
}
/* }}} */

/* {{{ proto int sqlite_num_rows(resource result)
   Returns the number of rows in a result set */
PHP_FUNCTION(sqlite_num_rows)
{
	zval *zres;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zres)) {
		return;
	}

	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	RETURN_LONG(res->nrows);
	
}
/* }}} */

/* {{{ proto int sqlite_num_fields(resource result)
   Returns the number of fields in a result set */
PHP_FUNCTION(sqlite_num_fields)
{
	zval *zres;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zres)) {
		return;
	}

	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	RETURN_LONG(res->ncolumns);
	
}
/* }}} */

/* {{{ proto string sqlite_field_name(resource result, int field)
   Returns the name of a particular field */
PHP_FUNCTION(sqlite_field_name)
{
	zval *zres;
	struct php_sqlite_result *res;
	int field;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zres, &field)) {
		return;
	}

	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	if (field < 0 || field >= res->ncolumns) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "field %d out of range", field);
		RETURN_FALSE;
	}
	
	RETURN_STRING(res->table[field], 1);	
}
/* }}} */

/* {{{ proto bool sqlite_seek(resource result, int row)
   Seek to a particular row number */
PHP_FUNCTION(sqlite_seek)
{
	zval *zres;
	struct php_sqlite_result *res;
	int row;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zres, &row)) {
		return;
	}

	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	if (row < 1 || row >= res->nrows) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "row %d out of range", row);
		RETURN_FALSE;
	}

	res->curr_row = row;
	RETURN_TRUE;
}
/* }}} */

