/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#if DBA_GDBM
#include "php_gdbm.h"

#ifdef GDBM_INCLUDE_FILE
#include GDBM_INCLUDE_FILE
#endif

#define GDBM_DATA dba_gdbm_data *dba = info->dbf
#define GDBM_GKEY datum gkey; gkey.dptr = (char *) key; gkey.dsize = keylen

typedef struct {
	GDBM_FILE dbf;
	datum nextkey;
} dba_gdbm_data;

DBA_OPEN_FUNC(gdbm)
{
	GDBM_FILE dbf;
	int gmode = 0;
	int filemode = 0644;

	gmode = info->mode == DBA_READER ? GDBM_READER :
		info->mode == DBA_WRITER ? GDBM_WRITER :
		info->mode == DBA_CREAT  ? GDBM_WRCREAT :
		info->mode == DBA_TRUNC ? GDBM_NEWDB : -1;

	if(gmode == -1)
		return FAILURE; /* not possible */

	if(info->argc > 0) {
		filemode = zval_get_long(&info->argv[0]);
	}

	dbf = gdbm_open(info->path, 0, gmode, filemode, NULL);

	if(dbf) {
		info->dbf = pemalloc(sizeof(dba_gdbm_data), info->flags&DBA_PERSISTENT);
		memset(info->dbf, 0, sizeof(dba_gdbm_data));
		((dba_gdbm_data *) info->dbf)->dbf = dbf;
		return SUCCESS;
	}
	*error = (char *)gdbm_strerror(gdbm_errno);
	return FAILURE;
}

DBA_CLOSE_FUNC(gdbm)
{
	GDBM_DATA;

	if(dba->nextkey.dptr) free(dba->nextkey.dptr);
	gdbm_close(dba->dbf);
	pefree(dba, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(gdbm)
{
	GDBM_DATA;
	datum gval;
	char *new = NULL;

	GDBM_GKEY;
	gval = gdbm_fetch(dba->dbf, gkey);
	if(gval.dptr) {
		if(newlen) *newlen = gval.dsize;
		new = estrndup(gval.dptr, gval.dsize);
		free(gval.dptr);
	}
	return new;
}

DBA_UPDATE_FUNC(gdbm)
{
	datum gval;
	GDBM_DATA;

	GDBM_GKEY;
	gval.dptr = (char *) val;
	gval.dsize = vallen;

	switch (gdbm_store(dba->dbf, gkey, gval, mode == 1 ? GDBM_INSERT : GDBM_REPLACE)) {
		case 0:
			return SUCCESS;
		case 1:
			return FAILURE;
		case -1:
			php_error_docref2(NULL, key, val, E_WARNING, "%s", gdbm_strerror(gdbm_errno));
			return FAILURE;
		default:
			php_error_docref2(NULL, key, val, E_WARNING, "Unknown return value");
			return FAILURE;
	}
}

DBA_EXISTS_FUNC(gdbm)
{
	GDBM_DATA;
	GDBM_GKEY;

	return gdbm_exists(dba->dbf, gkey) ? SUCCESS : FAILURE;
}

DBA_DELETE_FUNC(gdbm)
{
	GDBM_DATA;
	GDBM_GKEY;

	return gdbm_delete(dba->dbf, gkey) == -1 ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(gdbm)
{
	GDBM_DATA;
	datum gkey;
	char *key = NULL;

	if(dba->nextkey.dptr) {
		free(dba->nextkey.dptr);
	}

	gkey = gdbm_firstkey(dba->dbf);
	if(gkey.dptr) {
		key = estrndup(gkey.dptr, gkey.dsize);
		if(newlen) *newlen = gkey.dsize;
		dba->nextkey = gkey;
	} else {
		dba->nextkey.dptr = NULL;
	}
	return key;
}

DBA_NEXTKEY_FUNC(gdbm)
{
	GDBM_DATA;
	char *nkey = NULL;
	datum gkey;

	if(!dba->nextkey.dptr) return NULL;

	gkey = gdbm_nextkey(dba->dbf, dba->nextkey);
	free(dba->nextkey.dptr);
	if(gkey.dptr) {
		nkey = estrndup(gkey.dptr, gkey.dsize);
		if(newlen) *newlen = gkey.dsize;
		dba->nextkey = gkey;
	} else {
		dba->nextkey.dptr = NULL;
	}
	return nkey;
}

DBA_OPTIMIZE_FUNC(gdbm)
{
	GDBM_DATA;
	gdbm_reorganize(dba->dbf);
	return SUCCESS;
}

DBA_SYNC_FUNC(gdbm)
{
	GDBM_DATA;

	gdbm_sync(dba->dbf);
	return SUCCESS;
}

DBA_INFO_FUNC(gdbm)
{
	return estrdup(gdbm_version);
}

#endif
