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

#if DBA_DB3
#include "php_db3.h"
#include <sys/stat.h>

#include <string.h>
#ifdef DB3_INCLUDE_FILE
#include DB3_INCLUDE_FILE
#else
#include <db.h>
#endif

#define DB3_DATA dba_db3_data *dba = info->dbf
#define DB3_GKEY \
	DBT gkey; \
	memset(&gkey, 0, sizeof(gkey)); \
	gkey.data = (char *) key; gkey.size = keylen

typedef struct {
	DB *dbp;
	DBC *cursor;
} dba_db3_data;

DBA_OPEN_FUNC(db3)
{
	DB *dbp = NULL;
	DBTYPE type;
	int gmode = 0;
	int filemode = 0644;
	struct stat check_stat;
	int s = VCWD_STAT(info->path, &check_stat);

	type =  info->mode == DBA_READER ? DB_UNKNOWN :
		info->mode == DBA_TRUNC ? DB_BTREE :
		s? DB_BTREE : DB_UNKNOWN;
	  
	gmode = info->mode == DBA_READER ? DB_RDONLY :
		(info->mode == DBA_CREAT && s) ? DB_CREATE : 
		(info->mode == DBA_CREAT && !s) ? 0 :
		info->mode == DBA_WRITER ? 0         : 
		info->mode == DBA_TRUNC ? DB_CREATE | DB_TRUNCATE : -1;

	if (gmode == -1)
		return FAILURE; /* not possible */

	if (info->argc > 0) {
		convert_to_long_ex(info->argv[0]);
		filemode = Z_LVAL_PP(info->argv[0]);
	}

	if (db_create(&dbp, NULL, 0) == 0 &&
			dbp->open(dbp, info->path, NULL, type, gmode, filemode) == 0) {
		dba_db3_data *data;

		data = emalloc(sizeof(*data));
		if (!data) {
			*error = "Out of memory";
			return FAILURE;
		}
		data->dbp = dbp;
		data->cursor = NULL;
		info->dbf = data;
		
		return SUCCESS;
	} else if (dbp != NULL) {
		dbp->close(dbp, 0);
	}

	return FAILURE;
}

DBA_CLOSE_FUNC(db3)
{
	DB3_DATA;
	
	if (dba->cursor) dba->cursor->c_close(dba->cursor);
	dba->dbp->close(dba->dbp, 0);
	efree(dba);
}

DBA_FETCH_FUNC(db3)
{
	DBT gval;
	char *new = NULL;
	DB3_DATA;
	DB3_GKEY;
	
	memset(&gval, 0, sizeof(gval));
	if (!dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		if (newlen) *newlen = gval.size;
		new = estrndup(gval.data, gval.size);
	}
	return new;
}

DBA_UPDATE_FUNC(db3)
{
	DBT gval;
	DB3_DATA;
	DB3_GKEY;
	
	memset(&gval, 0, sizeof(gval));
	gval.data = (char *) val;
	gval.size = vallen;

	if (!dba->dbp->put(dba->dbp, NULL, &gkey, &gval, 
				mode == 1 ? DB_NOOVERWRITE : 0)) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_EXISTS_FUNC(db3)
{
	DBT gval;
	DB3_DATA;
	DB3_GKEY;
	
	memset(&gval, 0, sizeof(gval));
	if (!dba->dbp->get(dba->dbp, NULL, &gkey, &gval, 0)) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(db3)
{
	DB3_DATA;
	DB3_GKEY;

	return dba->dbp->del(dba->dbp, NULL, &gkey, 0) ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(db3)
{
	DB3_DATA;

	if (dba->cursor) {
		dba->cursor->c_close(dba->cursor);
	}

	dba->cursor = NULL;
	if (dba->dbp->cursor(dba->dbp, NULL, &dba->cursor, 0) != 0) {
		return NULL;
	}

	/* we should introduce something like PARAM_PASSTHRU... */
	return dba_nextkey_db3(info, newlen TSRMLS_CC);
}

DBA_NEXTKEY_FUNC(db3)
{
	DB3_DATA;
	DBT gkey, gval;
	char *nkey = NULL;
	
	memset(&gkey, 0, sizeof(gkey));
	memset(&gval, 0, sizeof(gval));

	if (dba->cursor->c_get(dba->cursor, &gkey, &gval, DB_NEXT) == 0) {
		if (gkey.data) {
			nkey = estrndup(gkey.data, gkey.size);
			if (newlen) *newlen = gkey.size;
		}
	}

	return nkey;
}

DBA_OPTIMIZE_FUNC(db3)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(db3)
{
	DB3_DATA;

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
