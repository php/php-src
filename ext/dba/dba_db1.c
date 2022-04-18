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
   | Author: Shen Cheng-Da <cdsheen@gmail.com>                            |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef DBA_DB1
#include "php_db1.h"

#ifdef DB1_INCLUDE_FILE
#include DB1_INCLUDE_FILE
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
	DB  *dbp;
} dba_db1_data;

DBA_OPEN_FUNC(db1)
{
	dba_db1_data	*dba;
	DB		*db;

	int gmode;
	int filemode = info->file_permission;

	gmode = 0;
	switch (info->mode) {
		case DBA_READER:
			gmode = O_RDONLY;
			break;
		case DBA_WRITER:
			gmode = O_RDWR;
			break;
		case DBA_CREAT:
			gmode = O_RDWR | O_CREAT;
			break;
		case DBA_TRUNC:
			gmode = O_RDWR | O_CREAT | O_TRUNC;
			break;
		default:
			return FAILURE; /* not possible */
	}

	db = dbopen((char *)info->path, gmode, filemode, DB_HASH, NULL);

	if (db == NULL) {
		return FAILURE;
	}

	dba = pemalloc(sizeof(*dba), info->flags&DBA_PERSISTENT);
	dba->dbp = db;

	info->dbf = dba;

	return SUCCESS;
}

DBA_CLOSE_FUNC(db1)
{
	dba_db1_data *dba = info->dbf;
	dba->dbp->close(dba->dbp);
	pefree(info->dbf, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(db1)
{
	dba_db1_data *dba = info->dbf;
	DBT gval;
	DBT gkey;

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	memset(&gval, 0, sizeof(gval));
	if (dba->dbp->get(dba->dbp, &gkey, &gval, 0) == RET_SUCCESS) {
		return zend_string_init(gval.data, gval.size, /* persistent */ false);
	}
	return NULL;
}

DBA_UPDATE_FUNC(db1)
{
	dba_db1_data *dba = info->dbf;
	DBT gval;
	DBT gkey;

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	gval.data = ZSTR_VAL(val);
	gval.size = ZSTR_LEN(val);

	return dba->dbp->put(dba->dbp, &gkey, &gval, mode == 1 ? R_NOOVERWRITE : 0) != RET_SUCCESS ? FAILURE : SUCCESS;
}

DBA_EXISTS_FUNC(db1)
{
	dba_db1_data *dba = info->dbf;
	DBT gval;
	DBT gkey;

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	return dba->dbp->get(dba->dbp, &gkey, &gval, 0) != RET_SUCCESS ? FAILURE : SUCCESS;
}

DBA_DELETE_FUNC(db1)
{
	dba_db1_data *dba = info->dbf;
	DBT gkey;

	gkey.data = ZSTR_VAL(key);
	gkey.size = ZSTR_LEN(key);

	return dba->dbp->del(dba->dbp, &gkey, 0) != RET_SUCCESS ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(db1)
{
	dba_db1_data *dba = info->dbf;
	DBT gkey;
	DBT gval;

	memset(&gkey, 0, sizeof(gkey));
	memset(&gval, 0, sizeof(gval));

	if (dba->dbp->seq(dba->dbp, &gkey, &gval, R_FIRST) == RET_SUCCESS) {
		return zend_string_init(gkey.data, gkey.size, /* persistent */ false);
	}
	return NULL;
}

DBA_NEXTKEY_FUNC(db1)
{
	dba_db1_data *dba = info->dbf;
	DBT gkey;
	DBT gval;

	memset(&gkey, 0, sizeof(gkey));
	memset(&gval, 0, sizeof(gval));

	if (dba->dbp->seq(dba->dbp, &gkey, &gval, R_NEXT) == RET_SUCCESS) {
		return zend_string_init(gkey.data, gkey.size, /* persistent */ false);
	}
	return NULL;
}

DBA_OPTIMIZE_FUNC(db1)
{
	/* dummy */
	return SUCCESS;
}

DBA_SYNC_FUNC(db1)
{
	return SUCCESS;
}

DBA_INFO_FUNC(db1)
{
	return estrdup(DB1_VERSION);
}

#endif
