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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef DBA_DB2
#include "php_db2.h"
#include <sys/stat.h>

#include <string.h>
#ifdef DB2_INCLUDE_FILE
#include DB2_INCLUDE_FILE
#endif

typedef struct {
	DB *dbp;
	DBC *cursor;
} dba_db2_data;

DBA_OPEN_FUNC(db2)
{
	DB *dbp;
	DBTYPE type;
	int gmode = 0;
	int filemode = info->file_permission;
	struct stat check_stat;
	int s = VCWD_STAT(ZSTR_VAL(info->path), &check_stat);

	if (!s && !check_stat.st_size) {
		info->mode = DBA_TRUNC; /* force truncate */
	}

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

	if (db_open(ZSTR_VAL(info->path), type, gmode, filemode, NULL, NULL, &dbp)) {
		return FAILURE;
	}

	info->dbf = pemalloc(sizeof(dba_db2_data), info->flags&DBA_PERSISTENT);
	memset(info->dbf, 0, sizeof(dba_db2_data));
	((dba_db2_data *) info->dbf)->dbp = dbp;
	return SUCCESS;
}

DBA_CLOSE_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;

	if (dba->cursor)
		dba->cursor->c_close(dba->cursor);
	dba->dbp->close(dba->dbp, 0);
	pefree(dba, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;
	DBT gval = {0};
	DBT gkey = {0};

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	if (dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		return NULL;
	}

	return zend_string_init(gval.data, gval.size, /* persistent */ false);
}

DBA_UPDATE_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;
	DBT gval = {0};
	DBT gkey = {0};

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	gval.data = ZSTR_VAL(val);
	gval.size = ZSTR_LEN(val);

	if (dba->dbp->put(dba->dbp, NULL, &gkey, &gval,
				mode == 1 ? DB_NOOVERWRITE : 0)) {
		return FAILURE;
	}
	return SUCCESS;
}

DBA_EXISTS_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;
	DBT gval = {0};
	DBT gkey = {0};

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	if (dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		return FAILURE;
	}
	return SUCCESS;
}

DBA_DELETE_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;
	DBT gkey = {0};

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	return dba->dbp->del(dba->dbp, NULL, &gkey, 0) ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;

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

	return dba_nextkey_db2(info);
}

DBA_NEXTKEY_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;
	DBT gkey = {0}, gval = {0};

	if (dba->cursor->c_get(dba->cursor, &gkey, &gval, DB_NEXT)
			|| !gkey.data) {
		return NULL;
	}

	return zend_string_init(gkey.data, gkey.size, /* persistent */ false);
}

DBA_OPTIMIZE_FUNC(db2)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(db2)
{
	dba_db2_data *dba = info->dbf;

	return dba->dbp->sync(dba->dbp, 0) ? FAILURE : SUCCESS;
}

DBA_INFO_FUNC(db2)
{
	return estrdup(DB_VERSION_STRING);
}

#endif
