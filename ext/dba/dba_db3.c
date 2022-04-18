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
#include "config.h"
#endif

#include "php.h"

#ifdef DBA_DB3
#include "php_db3.h"
#include <sys/stat.h>

#include <string.h>
#ifdef DB3_INCLUDE_FILE
#include DB3_INCLUDE_FILE
#else
#include <db.h>
#endif

static void php_dba_db3_errcall_fcn(
#if (DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 3))
	const DB_ENV *dbenv,
#endif
	const char *errpfx, const char *msg)
{

	php_error_docref(NULL, E_NOTICE, "%s%s", errpfx?errpfx:"", msg);
}

typedef struct {
	DB *dbp;
	DBC *cursor;
} dba_db3_data;

DBA_OPEN_FUNC(db3)
{
	DB *dbp = NULL;
	DBTYPE type;
	int gmode = 0, err;
	int filemode = info->file_permission;
	struct stat check_stat;
	int s = VCWD_STAT(info->path, &check_stat);

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
		return FAILURE; /* not possible */
	}

#ifdef DB_FCNTL_LOCKING
	gmode |= DB_FCNTL_LOCKING;
#endif

	if ((err=db_create(&dbp, NULL, 0)) == 0) {
	    dbp->set_errcall(dbp, php_dba_db3_errcall_fcn);
		if(
#if (DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 1))
			(err=dbp->open(dbp, 0, info->path, NULL, type, gmode, filemode)) == 0) {
#else
			(err=dbp->open(dbp, info->path, NULL, type, gmode, filemode)) == 0) {
#endif
			dba_db3_data *data;

			data = pemalloc(sizeof(*data), info->flags&DBA_PERSISTENT);
			data->dbp = dbp;
			data->cursor = NULL;
			info->dbf = data;

			return SUCCESS;
		} else {
			dbp->close(dbp, 0);
			*error = db_strerror(err);
		}
	} else {
		*error = db_strerror(err);
	}

	return FAILURE;
}

DBA_CLOSE_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;

	if (dba->cursor) dba->cursor->c_close(dba->cursor);
	dba->dbp->close(dba->dbp, 0);
	pefree(dba, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;
	DBT gval;
	DBT gkey;

	memset(&gkey, 0, sizeof(gkey));
	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	memset(&gval, 0, sizeof(gval));
	if (!dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		return zend_string_init(gval.data, gval.size, /* persistent */ false);
	}
	return NULL;
}

DBA_UPDATE_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;
	DBT gval;
	DBT gkey;

	memset(&gkey, 0, sizeof(gkey));
	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	memset(&gval, 0, sizeof(gval));
	gval.data = ZSTR_VAL(val);
	gval.size = ZSTR_LEN(val);

	if (!dba->dbp->put(dba->dbp, NULL, &gkey, &gval,
				mode == 1 ? DB_NOOVERWRITE : 0)) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_EXISTS_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;
	DBT gval;
	DBT gkey;

	memset(&gkey, 0, sizeof(gkey));
	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	memset(&gval, 0, sizeof(gval));
	if (!dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;
	DBT gkey;

	memset(&gkey, 0, sizeof(gkey));
	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	return dba->dbp->del(dba->dbp, NULL, &gkey, 0) ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;

	if (dba->cursor) {
		dba->cursor->c_close(dba->cursor);
	}

	dba->cursor = NULL;
	if (dba->dbp->cursor(dba->dbp, NULL, &dba->cursor, 0) != 0) {
		return NULL;
	}

	return dba_nextkey_db3(info);
}

DBA_NEXTKEY_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;
	DBT gkey, gval;

	memset(&gkey, 0, sizeof(gkey));
	memset(&gval, 0, sizeof(gval));

	if (dba->cursor->c_get(dba->cursor, &gkey, &gval, DB_NEXT) == 0) {
		if (gkey.data) {
			return zend_string_init(gkey.data, gkey.size, /* persistent */ false);
		}
	}

	return NULL;
}

DBA_OPTIMIZE_FUNC(db3)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(db3)
{
	dba_db3_data *dba = info->dbf;

	return dba->dbp->sync(dba->dbp, 0) ? FAILURE : SUCCESS;
}

DBA_INFO_FUNC(db3)
{
	return estrdup(DB_VERSION_STRING);
}

#endif
