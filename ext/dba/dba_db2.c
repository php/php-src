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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_DB2
#include "php_db2.h"
#include <sys/stat.h>

#include <string.h>
#ifdef DB2_INCLUDE_FILE
#include DB2_INCLUDE_FILE
#endif

#define DB2_DATA dba_db2_data *dba = info->dbf
#define DB2_GKEY \
	DBT gkey = {0}; \
	gkey.data = (char *) key; \
	gkey.size = keylen

typedef struct {
	DB *dbp;
	DBC *cursor;
} dba_db2_data;

DBA_OPEN_FUNC(db2)
{
	DB *dbp;
	DBTYPE type;
	int gmode = 0;
	int filemode = 0644;
	struct stat check_stat;
	int s = VCWD_STAT(info->path, &check_stat);

	type = info->mode == DBA_READER ? DB_UNKNOWN :
		info->mode == DBA_TRUNC ? DB_BTREE :
		s ? DB_BTREE : DB_UNKNOWN;
	  
	gmode = info->mode == DBA_READER ? DB_RDONLY :
		(info->mode == DBA_CREAT && s) ? DB_CREATE : 
		(info->mode == DBA_CREAT && !s) ? 0 :
		info->mode == DBA_WRITER ? 0         : 
		info->mode == DBA_TRUNC ? DB_CREATE | DB_TRUNCATE : -1;

	if (gmode == -1) {
		return FAILURE;/* not possible */
	}

	if (info->argc > 0) {
		convert_to_long_ex(info->argv[0]);
		filemode = Z_LVAL_PP(info->argv[0]);
	}

	if (db_open(info->path, type, gmode, filemode, NULL, NULL, &dbp)) {
		return FAILURE;
	}

	info->dbf = emalloc(sizeof(dba_db2_data));
	memset(info->dbf, 0, sizeof(dba_db2_data));
	((dba_db2_data *) info->dbf)->dbp = dbp;
	return SUCCESS;
}

DBA_CLOSE_FUNC(db2)
{
	DB2_DATA;
	
	if (dba->cursor) 
		dba->cursor->c_close(dba->cursor);
	dba->dbp->close(dba->dbp, 0);
	efree(dba);
}

DBA_FETCH_FUNC(db2)
{
	DBT gval = {0};
	DB2_DATA;
	DB2_GKEY;
	
	if (dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		return NULL;
	}

	if (newlen) *newlen = gval.size;
	return estrndup(gval.data, gval.size);
}

DBA_UPDATE_FUNC(db2)
{
	DBT gval = {0};
	DB2_DATA;
	DB2_GKEY;
	
	gval.data = (char *) val;
	gval.size = vallen;

	if (dba->dbp->put(dba->dbp, NULL, &gkey, &gval, 
				mode == 1 ? DB_NOOVERWRITE : 0)) {
		return FAILURE;
	}
	return SUCCESS;
}

DBA_EXISTS_FUNC(db2)
{
	DBT gval = {0};
	DB2_DATA;
	DB2_GKEY;
	
	if (dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		return FAILURE;
	}
	return SUCCESS;
}

DBA_DELETE_FUNC(db2)
{
	DB2_DATA;
	DB2_GKEY;

	return dba->dbp->del(dba->dbp, NULL, &gkey, 0) ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(db2)
{
	DB2_DATA;

	if (dba->cursor) {
		dba->cursor->c_close(dba->cursor);
		dba->cursor = NULL;
	}

#if (DB_VERSION_MAJOR > 2) || (DB_VERSION_MAJOR == 2 && DB_VERSION_MINOR > 6) || (DB_VERSION_MAJOR == 2 && DB_VERSION_MINOR == 6 && DB_VERSION_PATCH >= 4)
	if (dba->dbp->cursor(dba->dbp, NULL, &dba->cursor, 0)) {
#else
	if (dba->dbp->cursor(dba->dbp, NULL, &dba->cursor)) {
#endif
		return NULL;
	}

	/* we should introduce something like PARAM_PASSTHRU... */
	return dba_nextkey_db2(info, newlen TSRMLS_CC);
}

DBA_NEXTKEY_FUNC(db2)
{
	DB2_DATA;
	DBT gkey = {0}, gval = {0};

	if (dba->cursor->c_get(dba->cursor, &gkey, &gval, DB_NEXT)
			|| !gkey.data)
		return NULL;

	if (newlen) *newlen = gkey.size;
	return estrndup(gkey.data, gkey.size);
}

DBA_OPTIMIZE_FUNC(db2)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(db2)
{
	DB2_DATA;

	return dba->dbp->sync(dba->dbp, 0) ? FAILURE : SUCCESS;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
