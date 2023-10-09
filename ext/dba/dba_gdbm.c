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

#ifdef DBA_GDBM
#include "php_gdbm.h"

#ifdef GDBM_INCLUDE_FILE
#include GDBM_INCLUDE_FILE
#endif

typedef struct {
	GDBM_FILE dbf;
	datum nextkey;
} dba_gdbm_data;

DBA_OPEN_FUNC(gdbm)
{
	GDBM_FILE dbf;
	int gmode = 0;
	int filemode = info->file_permission;

	gmode = info->mode == DBA_READER ? GDBM_READER :
		info->mode == DBA_WRITER ? GDBM_WRITER :
		info->mode == DBA_CREAT  ? GDBM_WRCREAT :
		info->mode == DBA_TRUNC ? GDBM_NEWDB : -1;

	if(gmode == -1)
		return FAILURE; /* not possible */

	dbf = gdbm_open(ZSTR_VAL(info->path), /* int block_size */ 0, gmode, filemode, NULL);

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
	dba_gdbm_data *dba = info->dbf;

	if(dba->nextkey.dptr) free(dba->nextkey.dptr);
	gdbm_close(dba->dbf);
	pefree(dba, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;
	datum gval;
	datum gkey;
	zend_string *fetched_val = NULL;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);

	gval = gdbm_fetch(dba->dbf, gkey);
	if (gval.dptr) {
		fetched_val = zend_string_init(gval.dptr, gval.dsize, /* persistent */ false);
		free(gval.dptr);
	}
	return fetched_val;
}

DBA_UPDATE_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	gval.dptr = ZSTR_VAL(val);
	gval.dsize = ZSTR_LEN(val);

	switch (gdbm_store(dba->dbf, gkey, gval, mode == 1 ? GDBM_INSERT : GDBM_REPLACE)) {
		case 0:
			return SUCCESS;
		case 1:
			return FAILURE;
		case -1:
			// TODO Check when this happens and confirm this can even happen
			php_error_docref(NULL, E_WARNING, "%s", gdbm_strerror(gdbm_errno));
			return FAILURE;
		default:
			// TODO Convert this to an assertion failure
			php_error_docref(NULL, E_WARNING, "Unknown return value");
			return FAILURE;
	}
}

DBA_EXISTS_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);

	return gdbm_exists(dba->dbf, gkey) ? SUCCESS : FAILURE;
}

DBA_DELETE_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);

	return gdbm_delete(dba->dbf, gkey) == -1 ? FAILURE : SUCCESS;
}

DBA_FIRSTKEY_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;
	datum gkey;
	zend_string *key = NULL;

	if (dba->nextkey.dptr) {
		free(dba->nextkey.dptr);
	}

	gkey = gdbm_firstkey(dba->dbf);
	if (gkey.dptr) {
		key = zend_string_init(gkey.dptr, gkey.dsize, /* persistent */ false);
		dba->nextkey = gkey;
	} else {
		dba->nextkey.dptr = NULL;
	}
	return key;
}

DBA_NEXTKEY_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;
	zend_string *key = NULL;
	datum gkey;

	if(!dba->nextkey.dptr) { return NULL; }

	gkey = gdbm_nextkey(dba->dbf, dba->nextkey);
	free(dba->nextkey.dptr);
	if (gkey.dptr) {
		key = zend_string_init(gkey.dptr, gkey.dsize, /* persistent */ false);
		dba->nextkey = gkey;
	} else {
		dba->nextkey.dptr = NULL;
	}
	return key;
}

DBA_OPTIMIZE_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;

	gdbm_reorganize(dba->dbf);
	return SUCCESS;
}

DBA_SYNC_FUNC(gdbm)
{
	dba_gdbm_data *dba = info->dbf;

	gdbm_sync(dba->dbf);
	return SUCCESS;
}

DBA_INFO_FUNC(gdbm)
{
	return estrdup(gdbm_version);
}

#endif
