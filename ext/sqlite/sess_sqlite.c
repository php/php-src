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
   | Authors: John Coggeshall <john@php.net>                              |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "ext/session/php_session.h"
#include <sqlite.h>

#define PS_SQLITE_DATA sqlite *db = (sqlite*)PS_GET_MOD_DATA()
extern int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out);
extern int sqlite_decode_binary(const unsigned char *in, unsigned char *out);

#define CREATE_TBL_QUERY	"CREATE TABLE session_data(sess_id TEXT PRIMARY KEY, value TEXT, updated INTEGER)"
#define INSERT_QUERY		"REPLACE INTO session_data VALUES('%q', '%q', %d)"
#define SELECT_QUERY		"SELECT value FROM session_data WHERE sess_id='%q' LIMIT 1"
#define GC_QUERY			"DELETE FROM session_data WHERE (%d - updated) > %d"
#define DELETE_QUERY		"DELETE FROM session_data WHERE sess_id='%q'"

PS_FUNCS(sqlite);

ps_module ps_mod_sqlite = {
	PS_MOD(sqlite)
};

/* If you change the logic here, please also update the error message in
 * ps_sqlite_open() appropriately (code taken from ps_files_valid_key()) */

static int ps_sqlite_valid_key(const char *key)
{
	size_t len;
	const char *p;
	char c;
	int ret = 1;

	for (p = key; (c = *p); p++) {
		/* valid characters are a..z,A..Z,0..9 */
		if (!((c >= 'a' && c <= 'z')
				|| (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9')
				|| c == ','
				|| c == '-')) {
			ret = 0;
			break;
		}
	}

	len = p - key;
	
	if (len == 0)
		ret = 0;
	
	return ret;
}

PS_OPEN_FUNC(sqlite) 
{
    char *filepath;
	char *errmsg = NULL;
    int spath_len, sname_len, fp_len;
	sqlite *db;

	/* TODO: do we need a safe_mode check here? */
	db = sqlite_open(save_path, 0666, &errmsg);
	if (db == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: failed to open/create session database `%s' - %s", save_path, errmsg);
		sqlite_freemem(errmsg);
		return FAILURE;
	}

	/* allow up to 1 minute when busy */
	sqlite_busy_timeout(db, 60000);
    
    /* This will fail if the table already exists, but that's not a big problem. I'm
    unclear as to how to check for a table's existence in SQLite -- that would be better here. */
    sqlite_exec(db, CREATE_TBL_QUERY, NULL, NULL, NULL);
    
    PS_SET_MOD_DATA(db);

    return SUCCESS;
}

PS_CLOSE_FUNC(sqlite) 
{
	PS_SQLITE_DATA;

	sqlite_close(db);

	return SUCCESS;
}

PS_READ_FUNC(sqlite) 
{
	PS_SQLITE_DATA;
	char *query;
	sqlite_vm *vm;
	int colcount, result;
	const char **rowdata, **colnames;
	char *error;

	if (!ps_sqlite_valid_key(key)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: The session id contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,'");
		return FAILURE;
	}

	query = sqlite_mprintf(SELECT_QUERY, key);
	if (query == NULL) {
		/* no memory */
		return FAILURE;
	}

	if (sqlite_compile(db, query, NULL, &vm, &error) != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: Could not compile session read query: %s", error);
		sqlite_freemem(error);
		sqlite_freemem(query);
		return FAILURE;
	}

	switch ((result = sqlite_step(vm, &colcount, &rowdata, &colnames))) {
		case SQLITE_ROW:
			if (rowdata[0] == NULL) {
				*vallen = 0;
				*val = NULL;
			} else {
				*vallen = strlen(rowdata[0]);
				*val = emalloc(*vallen);
				*vallen = sqlite_decode_binary(rowdata[0], *val);
				(*val)[*vallen] = '\0';
			}
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: session read query failed: %s", error);
			sqlite_freemem(error);
			error = NULL;
	}
	
	if (SQLITE_OK != sqlite_finalize(vm, &error)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: session read: error %s", error);
		sqlite_freemem(error);
		error = NULL;
	}

	sqlite_freemem(query);
	
	return *val == NULL ? FAILURE : SUCCESS;
}

PS_WRITE_FUNC(sqlite) 
{
	PS_SQLITE_DATA;
	char *error;
	int result = SUCCESS;
	time_t t;
	char *binary;
	int binlen;
	
	t = time(NULL);

	binary = emalloc((256 * vallen + 1262) / 253);
	binlen = sqlite_encode_binary((const unsigned char*)val, vallen, binary);
	
	if (SQLITE_OK != sqlite_exec_printf(db, INSERT_QUERY, NULL, NULL, &error, key, binary, t)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: session write query failed: %s", error);
		sqlite_freemem(error);
		result = FAILURE;
	}

	efree(binary);

	return result;
}

PS_DESTROY_FUNC(sqlite) 
{
	PS_SQLITE_DATA;

	return SQLITE_OK == sqlite_exec_printf(db, DELETE_QUERY, NULL, NULL, NULL, key) ?
		SUCCESS : FAILURE;
}

PS_GC_FUNC(sqlite) 
{
	PS_SQLITE_DATA;
	time_t t = time(NULL);

	return SQLITE_OK == sqlite_exec_printf(db, GC_QUERY, NULL, NULL, NULL, t, maxlifetime) ?
		SUCCESS : FAILURE;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
