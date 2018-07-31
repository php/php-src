/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_DB4
#include "php_db4.h"
#include <sys/stat.h>

#include <string.h>
#ifdef DB4_INCLUDE_FILE
#include DB4_INCLUDE_FILE
#else
#include <db.h>
#endif

static void php_dba_db4_errcall_fcn(
#if (DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 3))
	const DB_ENV *dbenv,
#endif
	const char *errpfx, const char *msg)
{

#if (DB_VERSION_MAJOR == 5 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR == 8))
/* Bug 51086, Berkeley DB 4.8.26 */
/* This code suppresses a BDB 4.8+ error message, thus keeping PHP test compatibility */
	{
		const char *function = get_active_function_name();
		if (function && (!strcmp(function,"dba_popen") || !strcmp(function,"dba_open"))
			&& (!strncmp(msg, "fop_read_meta", sizeof("fop_read_meta")-1)
				|| !strncmp(msg, "BDB0004 fop_read_meta", sizeof("BDB0004 fop_read_meta")-1))) {
			return;
		}
	}
#endif

	php_error_docref(NULL, E_NOTICE, "%s%s", errpfx?errpfx:"", msg);
}

#define DB4_DATA dba_db4_data *dba = info->dbf
#define DB4_GKEY \
	DBT gkey; \
	memset(&gkey, 0, sizeof(gkey)); \
	gkey.data = (char *) key; gkey.size = keylen

typedef struct {
	DB *dbp;
	DBC *cursor;
} dba_db4_data;

DBA_OPEN_FUNC(db4)
{
	DB *dbp = NULL;
	DBTYPE type;
	int gmode = 0, err;
	int filemode = 0644;
	struct stat check_stat;
	int s = VCWD_STAT(info->path, &check_stat);

#if (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR <= 7)  /* Bug 51086 */
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
#else
	if (!s && !check_stat.st_size) {
		info->mode = DBA_CREAT; /* force creation */
	}

	type = info->mode == DBA_READER ? DB_UNKNOWN :
		(info->mode == DBA_TRUNC || info->mode == DBA_CREAT) ? DB_BTREE :
		s ? DB_BTREE : DB_UNKNOWN;

	gmode = info->mode == DBA_READER ? DB_RDONLY :
		info->mode == DBA_CREAT ? DB_CREATE :
		info->mode == DBA_WRITER ? 0         :
		info->mode == DBA_TRUNC ? DB_CREATE | DB_TRUNCATE : -1;
#endif

	if (gmode == -1) {
		return FAILURE; /* not possible */
	}

	if (info->flags & DBA_PERSISTENT) {
		gmode |= DB_THREAD;
	}

	if (info->argc > 0) {
		filemode = zval_get_long(&info->argv[0]);
	}

	if ((err=db_create(&dbp, NULL, 0)) == 0) {
	    dbp->set_errcall(dbp, php_dba_db4_errcall_fcn);
	    if (
#if (DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR >= 1))
			(err=dbp->open(dbp, 0, info->path, NULL, type, gmode, filemode)) == 0) {
#else
			(err=dbp->open(dbp, info->path, NULL, type, gmode, filemode)) == 0) {
#endif
			dba_db4_data *data;

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

DBA_CLOSE_FUNC(db4)
{
	DB4_DATA;

	if (dba->cursor) dba->cursor->c_close(dba->cursor);
	dba->dbp->close(dba->dbp, 0);
	pefree(dba, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(db4)
{
	DBT gval;
	char *new = NULL;
	DB4_DATA;
	DB4_GKEY;

	memset(&gval, 0, sizeof(gval));
	if (info->flags & DBA_PERSISTENT) {
		gval.flags |= DB_DBT_MALLOC;
	}
	if (!dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		if (newlen) *newlen = gval.size;
		new = estrndup(gval.data, gval.size);
		if (info->flags & DBA_PERSISTENT) {
			free(gval.data);
		}
	}
	return new;
}

DBA_UPDATE_FUNC(db4)
{
	DBT gval;
	DB4_DATA;
	DB4_GKEY;

	memset(&gval, 0, sizeof(gval));
	gval.data = (char *) val;
	gval.size = vallen;

	if (!dba->dbp->put(dba->dbp, NULL, &gkey, &gval,
				mode == 1 ? DB_NOOVERWRITE : 0)) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_EXISTS_FUNC(db4)
{
	DBT gval;
	DB4_DATA;
	DB4_GKEY;

	memset(&gval, 0, sizeof(gval));

	if (info->flags & DBA_PERSISTENT) {
		gval.flags |= DB_DBT_MALLOC;
	}

	if (!dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		if (info->flags & DBA_PERSISTENT) {
			free(gval.data);
		}
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(db4)
{
	DB4_DATA;
	DB4_GKEY;

	return dba->dbp->del(dba->dbp, NULL, &gkey, 0) ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(db4)
{
	DB4_DATA;

	if (dba->cursor) {
		dba->cursor->c_close(dba->cursor);
	}

	dba->cursor = NULL;
	if (dba->dbp->cursor(dba->dbp, NULL, &dba->cursor, 0) != 0) {
		return NULL;
	}

	/* we should introduce something like PARAM_PASSTHRU... */
	return dba_nextkey_db4(info, newlen);
}

DBA_NEXTKEY_FUNC(db4)
{
	DB4_DATA;
	DBT gkey, gval;
	char *nkey = NULL;

	memset(&gkey, 0, sizeof(gkey));
	memset(&gval, 0, sizeof(gval));

	if (info->flags & DBA_PERSISTENT) {
		gkey.flags |= DB_DBT_MALLOC;
		gval.flags |= DB_DBT_MALLOC;
	}
	if (dba->cursor && dba->cursor->c_get(dba->cursor, &gkey, &gval, DB_NEXT) == 0) {
		if (gkey.data) {
			nkey = estrndup(gkey.data, gkey.size);
			if (newlen) *newlen = gkey.size;
		}
		if (info->flags & DBA_PERSISTENT) {
			if (gkey.data) {
				free(gkey.data);
			}
			if (gval.data) {
				free(gval.data);
			}
		}
	}

	return nkey;
}

DBA_OPTIMIZE_FUNC(db4)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(db4)
{
	DB4_DATA;

	return dba->dbp->sync(dba->dbp, 0) ? FAILURE : SUCCESS;
}

DBA_INFO_FUNC(db4)
{
	return estrdup(DB_VERSION_STRING);
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
