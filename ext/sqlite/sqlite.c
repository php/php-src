/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   |          Tal Peer <tal@php.net>                                      |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+

   $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define PHP_SQLITE_MODULE_VERSION	"1.0"

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_sqlite.h"

#if HAVE_TIME_H
# include <time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sqlite.h>

#ifndef safe_emalloc
# define safe_emalloc(a,b,c) emalloc((a)*(b)+(c))
#endif

#ifndef ZEND_ENGINE_2
# define OnUpdateLong OnUpdateInt
#endif

ZEND_DECLARE_MODULE_GLOBALS(sqlite)

extern int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out);
extern int sqlite_decode_binary(const unsigned char *in, unsigned char *out);

static unsigned char arg3_force_ref[] = {3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

static int le_sqlite_db, le_sqlite_result, le_sqlite_pdb;

static inline void php_sqlite_strtoupper(char *s)
{
	while (*s!='\0') {
		*s = toupper(*s);
		s++;
	}
}

static inline void php_sqlite_strtolower(char *s)
{
	while (*s!='\0') {
		*s = tolower(*s);
		s++;
	}
}

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
STD_PHP_INI_ENTRY_EX("sqlite.assoc_case", "0", PHP_INI_ALL, OnUpdateLong, assoc_case, zend_sqlite_globals, sqlite_globals, display_link_numbers)
PHP_INI_END()
/* }}} */


#define DB_FROM_ZVAL(db, zv)	ZEND_FETCH_RESOURCE2(db, struct php_sqlite_db *, zv, -1, "sqlite database", le_sqlite_db, le_sqlite_pdb)

#define PHP_SQLITE_EMPTY_QUERY \
	if (!sql_len) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot execute empty query."); \
		RETURN_FALSE; \
	}

struct php_sqlite_result {
	struct php_sqlite_db *db;
	sqlite_vm *vm;
	int buffered;
	int ncolumns;
	int nrows;
	int curr_row;
	char **col_names;
	int alloc_rows;
	char **table;
	int mode;
};

struct php_sqlite_db {
	sqlite *db;
	int last_err_code;
	int is_persistent;
	int rsrc_id;

	HashTable callbacks;
};

struct php_sqlite_agg_functions {
	struct php_sqlite_db *db;
	int is_valid;
	zval *step;
	zval *fini;
};


enum { PHPSQLITE_ASSOC = 1, PHPSQLITE_NUM = 2, PHPSQLITE_BOTH = PHPSQLITE_ASSOC|PHPSQLITE_NUM };

function_entry sqlite_functions[] = {
	PHP_FE(sqlite_open, arg3_force_ref)
	PHP_FE(sqlite_popen, arg3_force_ref)
	PHP_FE(sqlite_close, NULL)
	PHP_FE(sqlite_query, NULL)
	PHP_FE(sqlite_exec, NULL)
	PHP_FE(sqlite_array_query, NULL)
	PHP_FE(sqlite_single_query, NULL)
	PHP_FE(sqlite_fetch_array, NULL)
	PHP_FE(sqlite_fetch_single, NULL)
	PHP_FALIAS(sqlite_fetch_string, sqlite_fetch_single, NULL)
	PHP_FE(sqlite_fetch_all, NULL)
	PHP_FE(sqlite_current, NULL)
	PHP_FE(sqlite_column, NULL)
	PHP_FE(sqlite_libversion, NULL)
	PHP_FE(sqlite_libencoding, NULL)
	PHP_FE(sqlite_changes, NULL)
	PHP_FE(sqlite_last_insert_rowid, NULL)
	PHP_FE(sqlite_num_rows, NULL)
	PHP_FE(sqlite_num_fields, NULL)
	PHP_FE(sqlite_field_name, NULL)
	PHP_FE(sqlite_seek, NULL)
	PHP_FE(sqlite_rewind, NULL)
	PHP_FE(sqlite_next, NULL)
	PHP_FE(sqlite_has_more, NULL)
	PHP_FE(sqlite_escape_string, NULL)
	PHP_FE(sqlite_busy_timeout, NULL)
	PHP_FE(sqlite_last_error, NULL)
	PHP_FE(sqlite_error_string, NULL)
	PHP_FE(sqlite_unbuffered_query, NULL)
	PHP_FE(sqlite_create_aggregate, NULL)
	PHP_FE(sqlite_create_function, NULL)
	PHP_FE(sqlite_udf_encode_binary, NULL)
	PHP_FE(sqlite_udf_decode_binary, NULL)
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
	PHP_RSHUTDOWN(sqlite),
	PHP_MINFO(sqlite),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_SQLITE_MODULE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_SQLITE
ZEND_GET_MODULE(sqlite)
#endif

static int php_sqlite_callback_invalidator(struct php_sqlite_agg_functions *funcs TSRMLS_DC)
{
	if (!funcs->is_valid) {
		return 0;
	}

	if (funcs->step) {
		zval_ptr_dtor(&funcs->step);
		funcs->step = NULL;
	}

	if (funcs->fini) {
		zval_ptr_dtor(&funcs->fini);
		funcs->fini = NULL;
	}

	funcs->is_valid = 0;

	return 0;
}


static void php_sqlite_callback_dtor(void *pDest)
{
	struct php_sqlite_agg_functions *funcs = (struct php_sqlite_agg_functions*)pDest;

	if (funcs->is_valid) {
		TSRMLS_FETCH();

		php_sqlite_callback_invalidator(funcs TSRMLS_CC);
	}
}

static ZEND_RSRC_DTOR_FUNC(php_sqlite_db_dtor)
{
	if (rsrc->ptr) {
		struct php_sqlite_db *db = (struct php_sqlite_db*)rsrc->ptr;
	
		sqlite_close(db->db);

		zend_hash_destroy(&db->callbacks);
		
		pefree(db, db->is_persistent);

		rsrc->ptr = NULL;
	}
}

static void real_result_dtor(struct php_sqlite_result *res TSRMLS_DC)
{
	int i, j, base;

	if (res->vm) {
		sqlite_finalize(res->vm, NULL);
	}

	if (res->table) {
		if (!res->buffered && res->nrows) {
			res->nrows = 1; /* only one row is stored */
		}
		for (i = 0; i < res->nrows; i++) {
			base = i * res->ncolumns;
			for (j = 0; j < res->ncolumns; j++) {
				if (res->table[base + j] != NULL) {
					efree(res->table[base + j]);
				}
			}
		}
		efree(res->table);
	}
	if (res->col_names) {
		for (j = 0; j < res->ncolumns; j++) {
			efree(res->col_names[j]);
		}
		efree(res->col_names);
	}

	if (res->db) {
		zend_list_delete(res->db->rsrc_id);
	}
	efree(res);
}

static ZEND_RSRC_DTOR_FUNC(php_sqlite_result_dtor)
{
	struct php_sqlite_result *res = (struct php_sqlite_result *)rsrc->ptr;
	real_result_dtor(res TSRMLS_CC);
}

static int php_sqlite_forget_persistent_id_numbers(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct php_sqlite_db *db = (struct php_sqlite_db*)rsrc->ptr;

	if (Z_TYPE_P(rsrc) != le_sqlite_pdb) {
		return 0;
	}

	/* prevent bad mojo if someone tries to use a previously registered function in the next request */
	zend_hash_apply(&db->callbacks, (apply_func_t)php_sqlite_callback_invalidator TSRMLS_CC);
	
	db->rsrc_id = FAILURE;

	/* don't leave pending commits hanging around */
	sqlite_exec(db->db, "ROLLBACK", NULL, NULL, NULL);
	
	return 0;
}

PHP_RSHUTDOWN_FUNCTION(sqlite)
{
	zend_hash_apply(&EG(persistent_list), (apply_func_t)php_sqlite_forget_persistent_id_numbers TSRMLS_CC);
	return SUCCESS;
}

/* {{{ PHP Function interface */
static void php_sqlite_generic_function_callback(sqlite_func *func, int argc, const char **argv)
{
	zval *retval = NULL;
	zval ***zargs = NULL;
	zval funcname;
	int i, res;
	char *callable = NULL, *errbuf=NULL;
	TSRMLS_FETCH();

	/* sanity check the args */
	if (argc == 0) {
		sqlite_set_result_error(func, "not enough parameters", -1);
		return;
	}
	
	ZVAL_STRING(&funcname, (char*)argv[0], 0);

	if (!zend_is_callable(&funcname, 0, &callable)) {
		spprintf(&errbuf, 0, "function `%s' is not callable", callable);
		sqlite_set_result_error(func, errbuf, -1);
		efree(errbuf);
		efree(callable);
		return;
	}
	efree(callable);
	
	if (argc > 1) {
		zargs = (zval ***)safe_emalloc((argc - 1), sizeof(zval **), 0);
		
		for (i = 0; i < argc-1; i++) {
			zargs[i] = emalloc(sizeof(zval *));
			MAKE_STD_ZVAL(*zargs[i]);
			ZVAL_STRING(*zargs[i], (char*)argv[i+1], 1);
		}
	}

	res = call_user_function_ex(EG(function_table),
			NULL,
			&funcname,
			&retval,
			argc-1,
			zargs,
			0, NULL TSRMLS_CC);

	if (res == SUCCESS) {
		if (retval == NULL) {
			sqlite_set_result_string(func, NULL, 0);
		} else {
			switch (Z_TYPE_P(retval)) {
				case IS_STRING:
					sqlite_set_result_string(func, Z_STRVAL_P(retval), Z_STRLEN_P(retval));
					break;
				case IS_LONG:
				case IS_BOOL:
					sqlite_set_result_int(func, Z_LVAL_P(retval));
					break;
				case IS_DOUBLE:
					sqlite_set_result_double(func, Z_DVAL_P(retval));
					break;
				case IS_NULL:
				default:
					sqlite_set_result_string(func, NULL, 0);
			}
		}
	} else {
		sqlite_set_result_error(func, "call_user_function_ex failed", -1);
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}

	if (zargs) {
		for (i = 0; i < argc-1; i++) {
			zval_ptr_dtor(zargs[i]);
			efree(zargs[i]);
		}
		efree(zargs);
	}
}
/* }}} */

/* {{{ callback for sqlite_create_function */
static void php_sqlite_function_callback(sqlite_func *func, int argc, const char **argv)
{
	zval *retval = NULL;
	zval ***zargs = NULL;
	int i, res;
	struct php_sqlite_agg_functions *funcs = sqlite_user_data(func);
	TSRMLS_FETCH();

	if (!funcs->is_valid) {
		sqlite_set_result_error(func, "this function has not been correctly defined for this request", -1);
		return;
	}

	if (argc > 0) {
		zargs = (zval ***)safe_emalloc(argc, sizeof(zval **), 0);
		
		for (i = 0; i < argc; i++) {
			zargs[i] = emalloc(sizeof(zval *));
			MAKE_STD_ZVAL(*zargs[i]);

			if (argv[i] == NULL) {
				ZVAL_NULL(*zargs[i]);
			} else {
				ZVAL_STRING(*zargs[i], (char*)argv[i], 1);
			}
		}
	}

	res = call_user_function_ex(EG(function_table),
			NULL,
			funcs->step,
			&retval,
			argc,
			zargs,
			0, NULL TSRMLS_CC);

	if (res == SUCCESS) {
		if (retval == NULL) {
			sqlite_set_result_string(func, NULL, 0);
		} else {
			switch (Z_TYPE_P(retval)) {
				case IS_STRING:
					/* TODO: for binary results, need to encode the string */
					sqlite_set_result_string(func, Z_STRVAL_P(retval), Z_STRLEN_P(retval));
					break;
				case IS_LONG:
				case IS_BOOL:
					sqlite_set_result_int(func, Z_LVAL_P(retval));
					break;
				case IS_DOUBLE:
					sqlite_set_result_double(func, Z_DVAL_P(retval));
					break;
				case IS_NULL:
				default:
					sqlite_set_result_string(func, NULL, 0);
			}
		}
	} else {
		sqlite_set_result_error(func, "call_user_function_ex failed", -1);
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}

	if (zargs) {
		for (i = 0; i < argc; i++) {
			zval_ptr_dtor(zargs[i]);
			efree(zargs[i]);
		}
		efree(zargs);
	}
}
/* }}} */

/* {{{ callback for sqlite_create_aggregate: step function */
static void php_sqlite_agg_step_function_callback(sqlite_func *func, int argc, const char **argv)
{
	zval *retval = NULL;
	zval ***zargs;
	zval **context_p;
	int i, res, zargc;
	struct php_sqlite_agg_functions *funcs = sqlite_user_data(func);
	TSRMLS_FETCH();

	if (!funcs->is_valid) {
		sqlite_set_result_error(func, "this function has not been correctly defined for this request", -1);
		return;
	}

	/* sanity check the args */
	if (argc < 1) {
		return;
	}
	
	zargc = argc + 1;
	zargs = (zval ***)safe_emalloc(zargc, sizeof(zval **), 0);
		
	/* first arg is always the context zval */
	context_p = (zval **)sqlite_aggregate_context(func, sizeof(*context_p));

	if (*context_p == NULL) {
		MAKE_STD_ZVAL(*context_p);
		(*context_p)->is_ref = 1;
		Z_TYPE_PP(context_p) = IS_NULL;
	}

	zargs[0] = context_p;

	/* copy the other args */
	for (i = 0; i < argc; i++) {
		zargs[i+1] = emalloc(sizeof(zval *));
		MAKE_STD_ZVAL(*zargs[i+1]);
		if (argv[i] == NULL) {
			ZVAL_NULL(*zargs[i+1]);
		} else {
			ZVAL_STRING(*zargs[i+1], (char*)argv[i], 1);
		}
	}

	res = call_user_function_ex(EG(function_table),
			NULL,
			funcs->step,
			&retval,
			zargc,
			zargs,
			0, NULL TSRMLS_CC);

	if (res != SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "call_user_function_ex failed");
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}

	if (zargs) {
		for (i = 1; i < zargc; i++) {
			zval_ptr_dtor(zargs[i]);
			efree(zargs[i]);
		}
		efree(zargs);
	}
}
/* }}} */

/* {{{ callback for sqlite_create_aggregate: finalize function */
static void php_sqlite_agg_fini_function_callback(sqlite_func *func)
{
	zval *retval = NULL;
	int res;
	struct php_sqlite_agg_functions *funcs = sqlite_user_data(func);
	zval **context_p;
	TSRMLS_FETCH();

	if (!funcs->is_valid) {
		sqlite_set_result_error(func, "this function has not been correctly defined for this request", -1);
		return;
	}
	
	context_p = (zval **)sqlite_aggregate_context(func, sizeof(*context_p));
	
	res = call_user_function_ex(EG(function_table),
			NULL,
			funcs->fini,
			&retval,
			1,
			&context_p,
			0, NULL TSRMLS_CC);

	if (res == SUCCESS) {
		if (retval == NULL) {
			sqlite_set_result_string(func, NULL, 0);
		} else {
			switch (Z_TYPE_P(retval)) {
				case IS_STRING:
					/* TODO: for binary results, need to encode the string */
					sqlite_set_result_string(func, Z_STRVAL_P(retval), Z_STRLEN_P(retval));
					break;
				case IS_LONG:
				case IS_BOOL:
					sqlite_set_result_int(func, Z_LVAL_P(retval));
					break;
				case IS_DOUBLE:
					sqlite_set_result_double(func, Z_DVAL_P(retval));
					break;
				case IS_NULL:
				default:
					sqlite_set_result_string(func, NULL, 0);
			}
		}
	} else {
		sqlite_set_result_error(func, "call_user_function_ex failed", -1);
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}

	zval_ptr_dtor(context_p);
}
/* }}} */

/* {{{ Authorization Callback */
static int php_sqlite_authorizer(void *autharg, int access_type, const char *arg3, const char *arg4,
		const char *arg5, const char *arg6)
{
	switch (access_type) {
		case SQLITE_COPY:
			if (strncmp(arg4, ":memory:", sizeof(":memory:") - 1)) {
				TSRMLS_FETCH();
				if (PG(safe_mode) && (!php_checkuid(arg4, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
					return SQLITE_DENY;
				}

				if (php_check_open_basedir(arg4 TSRMLS_CC)) {
					return SQLITE_DENY;
				}
			}
			return SQLITE_OK;
#ifdef SQLITE_ATTACH
		case SQLITE_ATTACH:
			if (strncmp(arg3, ":memory:", sizeof(":memory:") - 1)) {
				TSRMLS_FETCH();
				if (PG(safe_mode) && (!php_checkuid(arg3, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
					return SQLITE_DENY;
				}

				if (php_check_open_basedir(arg3 TSRMLS_CC)) {
					return SQLITE_DENY;
				}
			}
			return SQLITE_OK;
#endif

		default:
			/* access allowed */
			return SQLITE_OK;
	}
}
/* }}} */

static int init_sqlite_globals(zend_sqlite_globals *g)
{
	g->assoc_case = 0;
	return SUCCESS;
}

PHP_MINIT_FUNCTION(sqlite)
{
	ZEND_INIT_MODULE_GLOBALS(sqlite, init_sqlite_globals, NULL);
	
	REGISTER_INI_ENTRIES();

	le_sqlite_db = zend_register_list_destructors_ex(php_sqlite_db_dtor, NULL, "sqlite database", module_number);
	le_sqlite_pdb = zend_register_list_destructors_ex(NULL, php_sqlite_db_dtor, "sqlite database (persistent)", module_number);
	le_sqlite_result = zend_register_list_destructors_ex(php_sqlite_result_dtor, NULL, "sqlite result", module_number);

	REGISTER_LONG_CONSTANT("SQLITE_BOTH",	PHPSQLITE_BOTH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_NUM",	PHPSQLITE_NUM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_ASSOC",	PHPSQLITE_ASSOC, CONST_CS|CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("SQLITE_OK",				SQLITE_OK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_ERROR",			SQLITE_ERROR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_INTERNAL",		SQLITE_INTERNAL, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_PERM",			SQLITE_PERM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_ABORT",			SQLITE_ABORT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_BUSY",			SQLITE_BUSY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_LOCKED",			SQLITE_LOCKED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_NOMEM",			SQLITE_NOMEM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_READONLY",		SQLITE_READONLY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_INTERRUPT",		SQLITE_INTERRUPT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_IOERR",			SQLITE_IOERR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_CORRUPT",		SQLITE_CORRUPT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_NOTFOUND",		SQLITE_NOTFOUND, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_FULL",			SQLITE_FULL, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_CANTOPEN",		SQLITE_CANTOPEN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_PROTOCOL",		SQLITE_PROTOCOL, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_EMPTY",			SQLITE_EMPTY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_SCHEMA",			SQLITE_SCHEMA, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_TOOBIG",			SQLITE_TOOBIG, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_CONSTRAINT",		SQLITE_CONSTRAINT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_MISMATCH",		SQLITE_MISMATCH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_MISUSE",			SQLITE_MISUSE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_NOLFS",			SQLITE_NOLFS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_AUTH",			SQLITE_AUTH, CONST_CS|CONST_PERSISTENT);
#ifdef SQLITE_FORMAT
	REGISTER_LONG_CONSTANT("SQLITE_FORMAT",			SQLITE_FORMAT, CONST_CS|CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SQLITE_ROW",			SQLITE_ROW, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE_DONE",			SQLITE_DONE, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MINFO_FUNCTION(sqlite)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "SQLite support", "enabled");
	php_info_print_table_row(2, "PECL Module version", PHP_SQLITE_MODULE_VERSION " $Id$");
	php_info_print_table_row(2, "SQLite Library", sqlite_libversion());
	php_info_print_table_row(2, "SQLite Encoding", sqlite_libencoding());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

static struct php_sqlite_db *php_sqlite_open(char *filename, int mode, char *persistent_id, zval *return_value, zval *errmsg TSRMLS_DC)
{
	char *errtext = NULL;
	sqlite *sdb = NULL;
	struct php_sqlite_db *db = NULL;

	sdb = sqlite_open(filename, mode, &errtext);

	if (sdb == NULL) {

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);

		if (errmsg) {
			ZVAL_STRING(errmsg, errtext, 1);
		}

		sqlite_freemem(errtext);

		RETVAL_FALSE;
		return NULL;
	}

	db = (struct php_sqlite_db *)pemalloc(sizeof(struct php_sqlite_db), persistent_id ? 1 : 0);
	db->is_persistent = persistent_id ? 1 : 0;
	db->last_err_code = SQLITE_OK;
	db->db = sdb;

	zend_hash_init(&db->callbacks, 0, NULL, php_sqlite_callback_dtor, db->is_persistent);
	
	/* register the PHP functions */
	sqlite_create_function(sdb, "php", -1, php_sqlite_generic_function_callback, 0);

	/* set default busy handler; keep retrying up until 1 minute has passed,
	 * then fail with a busy status code */
	sqlite_busy_timeout(sdb, 60000);

	/* authorizer hook so we can enforce safe mode
	 * Note: the declaration of php_sqlite_authorizer is correct for 2.8.2 of libsqlite,
	 * and IS backwards binary compatible with earlier versions */
	sqlite_set_authorizer(sdb, php_sqlite_authorizer, NULL);
	
	db->rsrc_id = ZEND_REGISTER_RESOURCE(return_value, db, persistent_id ? le_sqlite_pdb : le_sqlite_db);

	if (persistent_id) {
		list_entry le;

		Z_TYPE(le) = le_sqlite_pdb;
		le.ptr = db;

		if (FAILURE == zend_hash_update(&EG(persistent_list), persistent_id,
					strlen(persistent_id)+1,
					(void *)&le, sizeof(le), NULL)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to register persistent resource");
		}
	}
	
	return db;
}

/* {{{ proto resource sqlite_popen(string filename [, int mode, string &errmessage])
   Opens a persistent handle to an SQLite database.  Will create the database if it does not exist */
PHP_FUNCTION(sqlite_popen)
{
	int mode = 0666;
	char *filename, *fullpath, *hashkey;
	long filename_len, hashkeylen;
	zval *errmsg = NULL;
	struct php_sqlite_db *db = NULL;
	list_entry *le;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz/",
				&filename, &filename_len, &mode, &errmsg)) {
		return;
	}
	if (errmsg) {
		zval_dtor(errmsg);
	}

	if (strncmp(filename, ":memory:", sizeof(":memory:") - 1)) {
		/* resolve the fully-qualified path name to use as the hash key */
		fullpath = expand_filepath(filename, NULL TSRMLS_CC);
	
		if (PG(safe_mode) && (!php_checkuid(fullpath, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
			RETURN_FALSE;
		}

		if (php_check_open_basedir(fullpath TSRMLS_CC)) {
			RETURN_FALSE;
		}
	} else {
		fullpath = estrndup(filename, filename_len);
	}

	hashkeylen = spprintf(&hashkey, 0, "sqlite_pdb_%s:%d", fullpath, mode);
	
	/* do we have an existing persistent connection ? */
	if (SUCCESS == zend_hash_find(&EG(persistent_list), hashkey, hashkeylen+1, (void*)&le)) {
		if (Z_TYPE_P(le) == le_sqlite_pdb) {
			db = (struct php_sqlite_db*)le->ptr;
			
			if (db->rsrc_id == FAILURE) {
				/* give it a valid resource id for this request */
				db->rsrc_id = ZEND_REGISTER_RESOURCE(return_value, db, le_sqlite_pdb);
			} else {
				int type;
				/* sanity check to ensure that the resource is still a valid regular resource
				 * number */
				if (_zend_list_find(db->rsrc_id, &type TSRMLS_CC) == db) {
					/* already accessed this request; map it */
					zend_list_addref(db->rsrc_id);
					ZVAL_RESOURCE(return_value, db->rsrc_id);
				} else {
					db->rsrc_id = ZEND_REGISTER_RESOURCE(return_value, db, le_sqlite_pdb);
				}
			}

			/* all set */
			efree(fullpath);
			efree(hashkey);
			return;
		}

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Some other type of persistent resource is using this hash key!?");
		RETURN_FALSE;
	}

	/* now we need to open the database */
	php_sqlite_open(fullpath, mode, hashkey, return_value, errmsg TSRMLS_CC);

	efree(fullpath);
	efree(hashkey);
}
/* }}} */

/* {{{ proto resource sqlite_open(string filename [, int mode, string &errmessage])
   Opens an SQLite database.  Will create the database if it does not exist */
PHP_FUNCTION(sqlite_open)
{
	int mode = 0666;
	char *filename, *fullpath = NULL;
	long filename_len;
	zval *errmsg = NULL;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz/",
				&filename, &filename_len, &mode, &errmsg)) {
		return;
	}
	if (errmsg) {
		zval_dtor(errmsg);
	}

	if (strncmp(filename, ":memory:", sizeof(":memory:") - 1)) {
		/* resolve the fully-qualified path name to use as the hash key */
		fullpath = expand_filepath(filename, NULL TSRMLS_CC);
	
		if (PG(safe_mode) && (!php_checkuid(fullpath, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
			efree(fullpath);
			RETURN_FALSE;
		}

		if (php_check_open_basedir(fullpath TSRMLS_CC)) {
			efree(fullpath);
			RETURN_FALSE;
		}
	}
	
	php_sqlite_open(fullpath ? fullpath : filename, mode, NULL, return_value, errmsg TSRMLS_CC);

	if (fullpath) {
		efree(fullpath);
	}
}
/* }}} */

/* {{{ proto void sqlite_busy_timeout(resource db, int ms)
   Set busy timeout duration. If ms <= 0, all busy handlers are disabled */
PHP_FUNCTION(sqlite_busy_timeout)
{
	zval *zdb;
	struct php_sqlite_db *db;
	long ms;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zdb, &ms)) {
		return;
	}

	DB_FROM_ZVAL(db, &zdb);

	sqlite_busy_timeout(db->db, ms);
}
/* }}} */

/* {{{ proto void sqlite_close(resource db)
   Closes an open sqlite database */
PHP_FUNCTION(sqlite_close)
{
	zval *zdb;
	struct php_sqlite_db *db;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}
	DB_FROM_ZVAL(db, &zdb);

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}

	DB_FROM_ZVAL(db, &zdb);

	zend_list_delete(Z_RESVAL_P(zdb));
}
/* }}} */

/* {{{ php_sqlite_fetch */
int php_sqlite_fetch(struct php_sqlite_result *rres TSRMLS_DC)
{
	const char **rowdata, **colnames;
	int ret, i, base;
	char *errtext = NULL, *colname;

next_row:
	ret = sqlite_step(rres->vm, &rres->ncolumns, &rowdata, &colnames);
	if (!rres->nrows) {
		/* first row - lets copy the column names */
		rres->col_names = safe_emalloc(rres->ncolumns, sizeof(char *), 0);
		for (i = 0; i < rres->ncolumns; i++) {
			colname = (char*)colnames[i];

			if (SQLITE_G(assoc_case) == 1) {
				php_sqlite_strtoupper(colname);
			} else if (SQLITE_G(assoc_case) == 2) {
				php_sqlite_strtolower(colname);
			}
			rres->col_names[i] = estrdup(colname);
		}
		if (!rres->buffered) {
			/* non buffered mode - also fetch memory for on single row */
			rres->table = safe_emalloc(rres->ncolumns, sizeof(char *), 0);
		}
	}

	switch (ret) {
		case SQLITE_ROW:
			if (rres->buffered) {
				/* add the row to our collection */
				if (rres->nrows + 1 >= rres->alloc_rows) {
					rres->alloc_rows = rres->alloc_rows ? rres->alloc_rows * 2 : 16;
					rres->table = erealloc(rres->table, rres->alloc_rows * rres->ncolumns * sizeof(char *));
				}
				base = rres->nrows * rres->ncolumns;
				for (i = 0; i < rres->ncolumns; i++) {
					if (rowdata[i]) {
						rres->table[base + i] = estrdup(rowdata[i]);
					} else {
						rres->table[base + i] = NULL;
					}
				}
				rres->nrows++;
				goto next_row;
			} else {
				/* non buffered: only fetch one row but first free data if not first row */
				if (rres->nrows++) {
					for (i = 0; i < rres->ncolumns; i++) {
						if (rres->table[i]) {
							efree(rres->table[i]);
						}
					}
				}
				for (i = 0; i < rres->ncolumns; i++) {
					if (rowdata[i]) {
						rres->table[i] = estrdup(rowdata[i]);
					} else {
						rres->table[i] = NULL;
					}
				}
			}
			ret = SQLITE_OK;
			break;

		case SQLITE_BUSY:
		case SQLITE_ERROR:
		case SQLITE_MISUSE:
		case SQLITE_DONE:
		default:
			if (rres->vm) {
				ret = sqlite_finalize(rres->vm, &errtext);
			}
			rres->vm = NULL;
			if (ret != SQLITE_OK) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
				sqlite_freemem(errtext);
			}
			break;
	}
	rres->db->last_err_code = ret;

	return ret;
}
/* }}} */

/* {{{ sqlite_query */
void sqlite_query(struct php_sqlite_db *db, char *sql, long sql_len, int mode, int buffered, zval *return_value, struct php_sqlite_result *rres TSRMLS_DC)
{
	struct php_sqlite_result res;
	int ret;
	char *errtext = NULL;
	const char *tail;

	memset(&res, 0, sizeof(res));
	res.buffered = buffered;
	res.mode = mode;

	ret = sqlite_compile(db->db, sql, &tail, &res.vm, &errtext);
	db->last_err_code = ret;

	if (ret != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
		sqlite_freemem(errtext);
		goto terminate;
	} else if (!res.vm) { /* emptry query */
terminate:
		if (return_value) {
			RETURN_FALSE;
		} else {
			return;
		}
	}

	if (!rres) {
		rres = (struct php_sqlite_result*)emalloc(sizeof(*rres));
	}
	memcpy(rres, &res, sizeof(*rres));
	rres->db = db;
	zend_list_addref(db->rsrc_id);
	

	/* now the result set is ready for stepping: get first row */
	if (php_sqlite_fetch(rres TSRMLS_CC) != SQLITE_OK) {
		real_result_dtor(rres TSRMLS_CC);
		if (return_value) {
			RETURN_FALSE;
		} else {
			return;	
		}
	}
	
	rres->curr_row = 0;

	if (return_value) {
		ZEND_REGISTER_RESOURCE(return_value, rres, le_sqlite_result);
	}
}
/* }}} */

/* {{{ proto resource sqlite_unbuffered_query(string query, resource db [ , int result_type ])
   Execute a query that does not prefetch and buffer all data */
PHP_FUNCTION(sqlite_unbuffered_query)
{
	zval *zdb;
	struct php_sqlite_db *db;
	char *sql;
	long sql_len;
	int mode = PHPSQLITE_BOTH;
	char *errtext = NULL;

	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
				ZEND_NUM_ARGS() TSRMLS_CC, "sr|l", &sql, &sql_len, &zdb, &mode) && 
			FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &zdb, &sql, &sql_len, &mode)) {
		return;
	}

	DB_FROM_ZVAL(db, &zdb);

	PHP_SQLITE_EMPTY_QUERY;

	/* avoid doing work if we can */
	if (!return_value_used) {
		db->last_err_code = sqlite_exec(db->db, sql, NULL, NULL, &errtext);

		if (db->last_err_code != SQLITE_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
			sqlite_freemem(errtext);
		}
		return;
	}
	
	sqlite_query(db, sql, sql_len, mode, 0, return_value, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto resource sqlite_query(string query, resource db [ , int result_type ])
   Executes a query against a given database and returns a result handle */
PHP_FUNCTION(sqlite_query)
{
	zval *zdb;
	struct php_sqlite_db *db;
	char *sql;
	long sql_len;
	int mode = PHPSQLITE_BOTH;
	char *errtext = NULL;

	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS() TSRMLS_CC, "sr|l", &sql, &sql_len, &zdb, &mode) && 
		FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &zdb, &sql, &sql_len, &mode)) {
		return;
	}
	DB_FROM_ZVAL(db, &zdb);

	PHP_SQLITE_EMPTY_QUERY;

	/* avoid doing work if we can */
	if (!return_value_used) {
		db->last_err_code = sqlite_exec(db->db, sql, NULL, NULL, &errtext);

		if (db->last_err_code != SQLITE_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
			sqlite_freemem(errtext);
		}
		return;
	}
	
	sqlite_query(db, sql, sql_len, mode, 1, return_value, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto boolean sqlite_exec(string query, resource db)
   Executes a result-less query against a given database */
PHP_FUNCTION(sqlite_exec)
{
	zval *zdb;
	struct php_sqlite_db *db;
	char *sql;
	long sql_len;
	char *errtext = NULL;

	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS() TSRMLS_CC, "sr", &sql, &sql_len, &zdb) && 
		FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zdb, &sql, &sql_len)) {
		return;
	}
	DB_FROM_ZVAL(db, &zdb);

	PHP_SQLITE_EMPTY_QUERY;

	db->last_err_code = sqlite_exec(db->db, sql, NULL, NULL, &errtext);

	if (db->last_err_code != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
		sqlite_freemem(errtext);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ php_sqlite_fetch_array */
static void php_sqlite_fetch_array(struct php_sqlite_result *res, int mode, zend_bool decode_binary, int move_next, zval *return_value TSRMLS_DC)
{
	int j, buffered = res->buffered;
	const char **rowdata, **colnames;
	
	/* check range of the row */
	if (res->curr_row >= res->nrows) {
		/* no more */
		RETURN_FALSE;
	}
	colnames = (const char**)res->col_names;
	if (res->buffered) {
		rowdata = (const char**)&res->table[res->curr_row * res->ncolumns];
	} else {
		rowdata = (const char**)res->table;
	}

	/* now populate the result */
	array_init(return_value);

	for (j = 0; j < res->ncolumns; j++) {
		zval *decoded;
		MAKE_STD_ZVAL(decoded);

		if (rowdata[j] == NULL) {
			ZVAL_NULL(decoded);
		} else if (decode_binary && rowdata[j][0] == '\x01') {
			Z_STRVAL_P(decoded) = emalloc(strlen(rowdata[j]));
			Z_STRLEN_P(decoded) = sqlite_decode_binary(rowdata[j]+1, Z_STRVAL_P(decoded));
			Z_STRVAL_P(decoded)[Z_STRLEN_P(decoded)] = '\0';
			Z_TYPE_P(decoded) = IS_STRING;
			if (!buffered) {
				efree((char*)rowdata[j]);
				rowdata[j] = NULL;
			}
		} else {
			ZVAL_STRING(decoded, (char*)rowdata[j], buffered);
			if (!buffered) {
				rowdata[j] = NULL;
			}
		}

		if (mode & PHPSQLITE_NUM) {
			if (mode & PHPSQLITE_ASSOC) {
				add_index_zval(return_value, j, decoded);
				ZVAL_ADDREF(decoded);
				add_assoc_zval(return_value, (char*)colnames[j], decoded);
			} else {
				add_next_index_zval(return_value, decoded);
			}
		} else {
			add_assoc_zval(return_value, (char*)colnames[j], decoded);
		}
	}

	if (move_next) {
		if (!res->buffered) {
			/* non buffered: fetch next row */
			php_sqlite_fetch(res TSRMLS_CC);
		}
		/* advance the row pointer */
		res->curr_row++;
	}
}
/* }}} */

/* {{{ php_sqlite_fetch_column */
static void php_sqlite_fetch_column(struct php_sqlite_result *res, zval *which, zend_bool decode_binary, zval *return_value TSRMLS_DC)
{
	int j;
	const char **rowdata, **colnames;

	/* check range of the row */
	if (res->curr_row >= res->nrows) {
		/* no more */
		RETURN_FALSE;
	}
	colnames = (const char**)res->col_names;

	if (Z_TYPE_P(which) == IS_LONG) {
		j = Z_LVAL_P(which);
	} else {
		convert_to_string_ex(&which);
		for (j = 0; j < res->ncolumns; j++) {
			if (!strcasecmp((char*)colnames[j], Z_STRVAL_P(which))) {
				break;
			}
		}
	}
	if (j < 0 || j >= res->ncolumns) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such column %d", j);
		RETURN_FALSE;
	}

	if (res->buffered) {
		rowdata = (const char**)&res->table[res->curr_row * res->ncolumns];
	} else {
		rowdata = (const char**)res->table;
	}

	if (rowdata[j] == NULL) {
		RETURN_NULL();
	} else if (decode_binary && rowdata[j][0] == '\x01') {
		int l = strlen(rowdata[j]);
		char *decoded = emalloc(l);
		l = sqlite_decode_binary(rowdata[j]+1, decoded);
		decoded[l] = '\0';
		RETVAL_STRINGL(decoded, l, 0);
		if (!res->buffered) {
			efree((char*)rowdata[j]);
			rowdata[j] = NULL;
		}
	} else {
		RETVAL_STRING((char*)rowdata[j], res->buffered);
		if (!res->buffered) {
			rowdata[j] = NULL;
		}
	}
}
/* }}} */

/* {{{ proto array sqlite_fetch_all(resource result [, int result_type, bool decode_binary])
   Fetches all rows from a result set as an array */
PHP_FUNCTION(sqlite_fetch_all)
{
	zval *zres, *ent;
	int mode = PHPSQLITE_BOTH;
	zend_bool decode_binary = 1;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|lb", &zres, &mode, &decode_binary)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);
	if (ZEND_NUM_ARGS() < 2) {
		mode = res->mode;
	}

	if (res->curr_row >= res->nrows && res->nrows) {
		if (!res->buffered) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "One or more rowsets were already returned");
		} else {
			res->curr_row = 0;
		}
	}

	array_init(return_value);

	while (res->curr_row < res->nrows) {
		MAKE_STD_ZVAL(ent);
		php_sqlite_fetch_array(res, mode, decode_binary, 1, ent TSRMLS_CC);
		add_next_index_zval(return_value, ent);
	}
}
/* }}} */

/* {{{ proto array sqlite_fetch_array(resource result [, int result_type, bool decode_binary])
   Fetches the next row from a result set as an array */
PHP_FUNCTION(sqlite_fetch_array)
{
	zval *zres;
	int mode = PHPSQLITE_BOTH;
	zend_bool decode_binary = 1;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|lb", &zres, &mode, &decode_binary)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);
	if (ZEND_NUM_ARGS() < 2) {
		mode = res->mode;
	}

	php_sqlite_fetch_array(res, mode, decode_binary, 1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto array sqlite_array_query(resource db, string query [ , int result_type, bool decode_binary ])
   Executes a query against a given database and returns an array */
PHP_FUNCTION(sqlite_array_query)
{
	zval *zdb, *ent;
	struct php_sqlite_db *db;
	struct php_sqlite_result *rres;
	char *sql;
	long sql_len;
	int mode = PHPSQLITE_BOTH;
	char *errtext = NULL;
	zend_bool decode_binary = 1;

	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS() TSRMLS_CC, "sr|lb", &sql, &sql_len, &zdb, &mode, &decode_binary) && 
		FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|lb", &zdb, &sql, &sql_len, &mode, &decode_binary)) {
		return;
	}
	DB_FROM_ZVAL(db, &zdb);

	PHP_SQLITE_EMPTY_QUERY;

	/* avoid doing work if we can */
	if (!return_value_used) {
		db->last_err_code = sqlite_exec(db->db, sql, NULL, NULL, &errtext);

		if (db->last_err_code != SQLITE_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
			sqlite_freemem(errtext);
		}
		return;
	}

	rres = (struct php_sqlite_result *)emalloc(sizeof(*rres));
	sqlite_query(db, sql, sql_len, mode, 0, NULL, rres TSRMLS_CC);
	if (db->last_err_code != SQLITE_OK) {
		if(!rres->vm) {
			/* no query happened - it's out responsibility to free it */
			efree(rres);
		}
		RETURN_FALSE;
	}

	array_init(return_value);

	while (rres->curr_row < rres->nrows) {
		MAKE_STD_ZVAL(ent);
		php_sqlite_fetch_array(rres, mode, decode_binary, 1, ent TSRMLS_CC);
		add_next_index_zval(return_value, ent);
	}
	real_result_dtor(rres TSRMLS_CC);
}
/* }}} */

/* {{{ php_sqlite_fetch_single */
static void php_sqlite_fetch_single(struct php_sqlite_result *res, zend_bool decode_binary, zval *return_value TSRMLS_DC)
{
	const char **rowdata;
	char *decoded;
	int decoded_len;
	
	/* check range of the row */
	if (res->curr_row >= res->nrows) {
		/* no more */
		RETURN_FALSE;
	}
	
	if (res->buffered) {
		rowdata = (const char**)&res->table[res->curr_row * res->ncolumns];
	} else {
		rowdata = (const char**)res->table;
	}

	if (decode_binary && rowdata[0] != NULL && rowdata[0][0] == '\x01') {
		decoded = emalloc(strlen(rowdata[0]));
		decoded_len = sqlite_decode_binary(rowdata[0]+1, decoded);
		if (!res->buffered) {
			efree((char*)rowdata[0]);
			rowdata[0] = NULL;
		}
	} else if (rowdata[0]) {
		decoded_len = strlen((char*)rowdata[0]);
		if (res->buffered) {
			decoded = estrndup((char*)rowdata[0], decoded_len);
		} else {
			decoded = (char*)rowdata[0];
			rowdata[0] = NULL;
		}
	} else {
		decoded = NULL;
		decoded_len = 0;
	}

	if (!res->buffered) {
		/* non buffered: fetch next row */
		php_sqlite_fetch(res TSRMLS_CC);
	}
	/* advance the row pointer */
	res->curr_row++;

	if (decoded == NULL) {
		RETURN_NULL();
	} else {
		RETURN_STRINGL(decoded, decoded_len, 0);
	}
}
/* }}} */

/* {{{ proto array sqlite_single_query(resource db, string query [ , bool single_row, bool decode_binary ])
   Executes a query against a given database and returns an array */
PHP_FUNCTION(sqlite_single_query)
{
	zval *zdb, *ent;
	struct php_sqlite_db *db;
	struct php_sqlite_result *rres;
	char *sql;
	long sql_len;
	char *errtext = NULL;
	zend_bool decode_binary = 1;
	zend_bool srow = 1;

	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS() TSRMLS_CC, "sr|bb", &sql, &sql_len, &zdb, &srow, &decode_binary) && 
		FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|bb", &zdb, &sql, &sql_len, &srow, &decode_binary)) {
		return;
	}
	DB_FROM_ZVAL(db, &zdb);

	PHP_SQLITE_EMPTY_QUERY;

	/* avoid doing work if we can */
	if (!return_value_used) {
		db->last_err_code = sqlite_exec(db->db, sql, NULL, NULL, &errtext);

		if (db->last_err_code != SQLITE_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
			sqlite_freemem(errtext);
		}
		return;
	}

	rres = (struct php_sqlite_result *)emalloc(sizeof(*rres));
	sqlite_query(db, sql, sql_len, PHPSQLITE_NUM, 0, NULL, rres TSRMLS_CC);
	if (db->last_err_code != SQLITE_OK) {
		efree(rres);
		RETURN_FALSE;
	}

	if (!srow) {
		array_init(return_value);
	}

	while (rres->curr_row < rres->nrows) {
		MAKE_STD_ZVAL(ent);
		php_sqlite_fetch_single(rres, decode_binary, ent TSRMLS_CC);

		/* if set and we only have 1 row in the result set, return the result as a string. */
		if (srow) {
			if (rres->curr_row == 1 && rres->curr_row >= rres->nrows) {
				*return_value = *ent;
				zval_copy_ctor(return_value);
				zval_dtor(ent);
				FREE_ZVAL(ent);
				break;
			} else {
				srow = 0;
				array_init(return_value);
			}
		}
		add_next_index_zval(return_value, ent);
	}

	real_result_dtor(rres TSRMLS_CC);
}
/* }}} */


/* {{{ proto string sqlite_fetch_array(resource result [, bool decode_binary])
   Fetches first column of a result set as a string */
PHP_FUNCTION(sqlite_fetch_single)
{
	zval *zres;
	zend_bool decode_binary = 1;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|b", &zres, &decode_binary)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	php_sqlite_fetch_single(res, decode_binary, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto array sqlite_fetch_array(resource result [, int result_type, bool decode_binary])
   Fetches the current row from a result set as an array */
PHP_FUNCTION(sqlite_current)
{
	zval *zres;
	int mode = PHPSQLITE_BOTH;
	zend_bool decode_binary = 1;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|lb", &zres, &mode, &decode_binary)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);
	if (ZEND_NUM_ARGS() < 2) {
		mode = res->mode;
	}

	php_sqlite_fetch_array(res, mode, decode_binary, 0, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed sqlite_column(resource result, mixed index_or_name [, bool decode_binary])
   Fetches a column from the current row of a result set */
PHP_FUNCTION(sqlite_column)
{
	zval *zres;
	zval *which;
	zend_bool decode_binary = 1;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|b", &zres, &which, &decode_binary)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	php_sqlite_fetch_column(res, which, decode_binary, return_value TSRMLS_CC);
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
	struct php_sqlite_db *db;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}

	DB_FROM_ZVAL(db, &zdb);

	RETURN_LONG(sqlite_changes(db->db));
}
/* }}} */

/* {{{ proto int sqlite_last_insert_rowid(resource db)
   Returns the rowid of the most recently inserted row */
PHP_FUNCTION(sqlite_last_insert_rowid)
{
	zval *zdb;
	struct php_sqlite_db *db;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}

	DB_FROM_ZVAL(db, &zdb);

	RETURN_LONG(sqlite_last_insert_rowid(db->db));
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

	if (res->buffered) {
		RETURN_LONG(res->nrows);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Row count is not available for unbuffered queries");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool sqlite_has_more(resource result)
   Returns whether or not more rows are available */
PHP_FUNCTION(sqlite_has_more)
{
	zval *zres;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zres)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	RETURN_BOOL(res->nrows && res->curr_row < res->nrows); /* curr_row may be -1 */
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

	RETURN_STRING(res->col_names[field], 1);
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

	if (!res->buffered) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot seek an unbuffered result set");
		RETURN_FALSE;
	}
	
	if (row < 0 || row >= res->nrows) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "row %d out of range", row);
		RETURN_FALSE;
	}

	res->curr_row = row;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool sqlite_rewind(resource result)
   Seek to first row number */
PHP_FUNCTION(sqlite_rewind)
{
	zval *zres;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zres)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	if (!res->buffered) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot seek an unbuffered result set");
		RETURN_FALSE;
	}
	
	if (!res->nrows) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "no rows received");
		RETURN_FALSE;
	}

	res->curr_row = 0;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool sqlite_next(resource result)
   Seek to next row number */
PHP_FUNCTION(sqlite_next)
{
	zval *zres;
	struct php_sqlite_result *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zres)) {
		return;
	}
	ZEND_FETCH_RESOURCE(res, struct php_sqlite_result *, &zres, -1, "sqlite result", le_sqlite_result);

	if (!res->buffered && res->vm) {
		php_sqlite_fetch(res TSRMLS_CC);
	}

	if (res->curr_row >= res->nrows) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "no more rows available");
		RETURN_FALSE;
	}

	res->curr_row++;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string sqlite_escape_string(string item)
   Escapes a string for use as a query parameter */
PHP_FUNCTION(sqlite_escape_string)
{
	char *string = NULL;
	long stringlen;
	char *ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &stringlen)) {
		return;
	}

	if (stringlen && (string[0] == '\x01' || memchr(string, '\0', stringlen) != NULL)) {
		/* binary string */
		int enclen;
		
		ret = emalloc( 1 + 5 + stringlen * ((float) 256 / (float) 253) );
		ret[0] = '\x01';
		enclen = sqlite_encode_binary((const unsigned char*)string, stringlen, ret+1);
		RETVAL_STRINGL(ret, enclen+1, 0);
		
	} else  {
		ret = sqlite_mprintf("%q", string);
		if (ret) {
			RETVAL_STRING(ret, 1);
			sqlite_freemem(ret);
		}
	}
}
/* }}} */

/* {{{ proto int sqlite_last_error(resource db)
   Returns the error code of the last error for a database */
PHP_FUNCTION(sqlite_last_error)
{
	zval *zdb;
	struct php_sqlite_db *db;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdb)) {
		return;
	}

	DB_FROM_ZVAL(db, &zdb);

	RETURN_LONG(db->last_err_code);
}
/* }}} */

/* {{{ proto string sqlite_error_string(int error_code)
   Returns the textual description of an error code */
PHP_FUNCTION(sqlite_error_string)
{
	long code;
	const char *msg;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &code)) {
		return;
	}
	
	msg = sqlite_error_string(code);

	if (msg) {
		RETURN_STRING((char*)msg, 1);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* manages duplicate registrations of a particular function, and
 * also handles the case where the db is using a persistent connection */
enum callback_prep_t { DO_REG, SKIP_REG, ERR };

static enum callback_prep_t prep_callback_struct(struct php_sqlite_db *db, int is_agg,
		char *funcname,
		zval *step, zval *fini, struct php_sqlite_agg_functions **funcs)
{
	struct php_sqlite_agg_functions *alloc_funcs, func_tmp;
	char *hashkey;
	int hashkeylen;
	enum callback_prep_t ret;

	hashkeylen = spprintf(&hashkey, 0, "%s-%s", is_agg ? "agg" : "reg", funcname);

	/* is it already registered ? */
	if (SUCCESS == zend_hash_find(&db->callbacks, hashkey, hashkeylen+1, (void*)&alloc_funcs)) {
		/* override the previous definition */

		if (alloc_funcs->is_valid) {
			/* release these */

			if (alloc_funcs->step) {
				zval_ptr_dtor(&alloc_funcs->step);
				alloc_funcs->step = NULL;
			}

			if (alloc_funcs->fini) {
				zval_ptr_dtor(&alloc_funcs->fini);
				alloc_funcs->fini = NULL;
			}
		}

		ret = SKIP_REG;
	} else {
		/* add a new one */
		func_tmp.db = db;

		ret = SUCCESS == zend_hash_update(&db->callbacks, hashkey, hashkeylen+1,
				(void*)&func_tmp, sizeof(func_tmp), (void**)&alloc_funcs) ? DO_REG : ERR;
	}

	efree(hashkey);

	MAKE_STD_ZVAL(alloc_funcs->step);
	*(alloc_funcs->step)  = *step;
	zval_copy_ctor(alloc_funcs->step);

	if (is_agg) {
		MAKE_STD_ZVAL(alloc_funcs->fini);
		*(alloc_funcs->fini) = *fini;
		zval_copy_ctor(alloc_funcs->fini);
	} else {
		alloc_funcs->fini = NULL;
	}
	alloc_funcs->is_valid = 1;
	*funcs = alloc_funcs;
	
	return ret;
}


/* {{{ proto bool sqlite_create_aggregate(resource db, string funcname, mixed step_func, mixed finalize_func[, long num_args])
    Registers an aggregated function for queries*/
PHP_FUNCTION(sqlite_create_aggregate)
{
	char *funcname = NULL;
	long funcname_len;
	zval *zstep, *zfinal, *zdb;
	struct php_sqlite_db *db;
	struct php_sqlite_agg_functions *funcs;
	char *callable = NULL;
	long num_args = -1;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rszz|l", &zdb, &funcname, &funcname_len, &zstep, &zfinal, &num_args)) {
		return;
	}
	DB_FROM_ZVAL(db, &zdb);

	if (!zend_is_callable(zstep, 0, &callable)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "step function `%s' is not callable", callable);
		efree(callable);
		return;
	}
	efree(callable);
	
	if (!zend_is_callable(zfinal, 0, &callable)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "finalize function `%s' is not callable", callable);
		efree(callable);
		return;
	}
	efree(callable);

	if (prep_callback_struct(db, 1, funcname, zstep, zfinal, &funcs) == DO_REG) {
		sqlite_create_aggregate(db->db, funcname, num_args,
				php_sqlite_agg_step_function_callback,
				php_sqlite_agg_fini_function_callback, funcs);
	}
	

}
/* }}} */

/* {{{ proto bool sqlite_create_function(resource db, string funcname, mixed callback[, long num_args])
    Registers a "regular" function for queries */
PHP_FUNCTION(sqlite_create_function)
{
	char *funcname = NULL;
	long funcname_len;
	zval *zcall, *zdb;
	struct php_sqlite_db *db;
	struct php_sqlite_agg_functions *funcs;
	char *callable = NULL;
	long num_args = -1;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsz|l", &zdb, &funcname, &funcname_len, &zcall, &num_args)) {
		return;
	}
	DB_FROM_ZVAL(db, &zdb);

	if (!zend_is_callable(zcall, 0, &callable)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function `%s' is not callable", callable);
		efree(callable);
		return;
	}
	efree(callable);
	
	if (prep_callback_struct(db, 0, funcname, zcall, NULL, &funcs) == DO_REG) {
		sqlite_create_function(db->db, funcname, num_args, php_sqlite_function_callback, funcs);
	}
}
/* }}} */

/* {{{ proto string sqlite_udf_encode_binary(string data)
   Apply binary encoding (if required) to a string to return from an UDF */
PHP_FUNCTION(sqlite_udf_encode_binary)
{
	char *data = NULL;
	long datalen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!", &data, &datalen)) {
		return;
	}

	if (data == NULL) {
		RETURN_NULL();
	}
	if (datalen && (data[0] == '\x01' || memchr(data, '\0', datalen) != NULL)) {
		/* binary string */
		int enclen;
		char *ret;
		
		ret = emalloc( 1 + 5 + datalen * ((float) 256 / (float) 253) );
		ret[0] = '\x01';
		enclen = sqlite_encode_binary((const unsigned char*)data, datalen, ret+1);
		RETVAL_STRINGL(ret, enclen+1, 0);
	} else {
		RETVAL_STRINGL(data, datalen, 1);
	}
}
/* }}} */

/* {{{ proto string sqlite_udf_decode_binary(string data)
   Decode binary encoding on a string parameter passed to an UDF */
PHP_FUNCTION(sqlite_udf_decode_binary)
{
	char *data = NULL;
	long datalen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!", &data, &datalen)) {
		return;
	}

	if (data == NULL) {
		RETURN_NULL();
	}
	if (datalen && data[0] == '\x01') {
		/* encoded string */
		int enclen;
		char *ret;
		
		ret = emalloc(datalen);
		enclen = sqlite_decode_binary((const unsigned char*)data+1, ret);
		ret[enclen] = '\0';
		RETVAL_STRINGL(ret, enclen, 0);
	} else {
		RETVAL_STRINGL(data, datalen, 1);
	}
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
