/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)

#include "ext/session/php_session.h"
#include "ext/standard/php_lcg.h"
#include <sqlite.h>
#define SQLITE_RETVAL(__r) ((__r) == SQLITE_OK ? SUCCESS : FAILURE)
#define PS_SQLITE_DATA sqlite *db = (sqlite*)PS_GET_MOD_DATA()
extern int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out);
extern int sqlite_decode_binary(const unsigned char *in, unsigned char *out);

PS_FUNCS(sqlite);

ps_module ps_mod_sqlite = {
	PS_MOD(sqlite)
};

PS_OPEN_FUNC(sqlite) 
{
	char *errmsg = NULL;
	sqlite *db;

	/* TODO: do we need a safe_mode check here? */
	db = sqlite_open(save_path, 0666, &errmsg);
	if (db == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				"SQLite: failed to open/create session database `%s' - %s", save_path, errmsg);
		sqlite_freemem(errmsg);
		return FAILURE;
	}

	/* allow up to 1 minute when busy */
	sqlite_busy_timeout(db, 60000);

	sqlite_exec(db, "PRAGMA default_synchronous = OFF", NULL, NULL, NULL);
	sqlite_exec(db, "PRAGMA count_changes = OFF", NULL, NULL, NULL);

	/* This will fail if the table already exists, but that's not a big problem. I'm
	   unclear as to how to check for a table's existence in SQLite -- that would be better here. */
	sqlite_exec(db, 
	    "CREATE TABLE session_data ("
	    "    sess_id PRIMARY KEY," 
	    "    value TEXT, "
	    "    updated INTEGER "
	    ")", NULL, NULL, NULL);

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
	const char *tail;
	sqlite_vm *vm;
	int colcount, result;
	const char **rowdata, **colnames;
	char *error;

	*val = NULL;
	*vallen = 0;
	
	query = sqlite_mprintf("SELECT value FROM session_data WHERE sess_id='%q' LIMIT 1", key);
	if (query == NULL) {
		/* no memory */
		return FAILURE;
	}

	if (sqlite_compile(db, query, &tail, &vm, &error) != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: Could not compile session read query: %s", error);
		sqlite_freemem(error);
		sqlite_freemem(query);
		return FAILURE;
	}

	switch ((result = sqlite_step(vm, &colcount, &rowdata, &colnames))) {
		case SQLITE_ROW:
			if (rowdata[0] != NULL) {
				*vallen = strlen(rowdata[0]);
				if (*vallen) {
					*val = emalloc(*vallen);
					*vallen = sqlite_decode_binary(rowdata[0], *val);
					(*val)[*vallen] = '\0';
				} else {
					*val = STR_EMPTY_ALLOC();
				}
			}
			break;
		default:
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
	time_t t;
	char *binary;
	int binlen;
	int rv;
	
	t = time(NULL);

	binary = safe_emalloc(1 + vallen / 254, 257, 3);
	binlen = sqlite_encode_binary((const unsigned char*)val, vallen, binary);
	
	rv = sqlite_exec_printf(db, "REPLACE INTO session_data VALUES('%q', '%q', %d)", NULL, NULL, &error, key, binary, t);
	if (rv != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite: session write query failed: %s", error);
		sqlite_freemem(error);
	}
	efree(binary);

	return SQLITE_RETVAL(rv);
}

PS_DESTROY_FUNC(sqlite) 
{
	int rv;
	PS_SQLITE_DATA;

	rv = sqlite_exec_printf(db, "DELETE FROM session_data WHERE sess_id='%q'", NULL, NULL, NULL, key);
	
	return SQLITE_RETVAL(rv);
}

PS_GC_FUNC(sqlite) 
{
	PS_SQLITE_DATA;
	int rv;
	time_t t = time(NULL);

	rv = sqlite_exec_printf(db, 
			"DELETE FROM session_data WHERE (%d - updated) > %d", 
			NULL, NULL, NULL, t, maxlifetime);

	/* because SQLite does not actually clear the deleted data from the database 
	 * we need to occassionaly do so manually to prevent the sessions database 
	 * from growing endlessly.
	 */
	if ((int) ((float) PS(gc_divisor) * PS(gc_divisor) * php_combined_lcg(TSRMLS_C)) < PS(gc_probability)) {
		rv = sqlite_exec_printf(db, "VACUUM", NULL, NULL, NULL);
	}
	return SQLITE_RETVAL(rv);
}

#endif /* HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION) */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
