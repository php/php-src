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

#ifdef DBA_DBM
#include "php_dbm.h"

#ifdef DBM_INCLUDE_FILE
#include DBM_INCLUDE_FILE
#endif
#ifdef DBA_GDBM
#include "php_gdbm.h"
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TRUNC_IT(extension, mode) \
	snprintf(buf, MAXPATHLEN, "%s" extension, ZSTR_VAL(info->path)); \
	buf[MAXPATHLEN-1] = '\0'; \
	if((fd = VCWD_OPEN_MODE(buf, O_CREAT | mode | O_WRONLY, filemode)) == -1) \
		return FAILURE; \
	close(fd);


typedef struct {
	datum nextkey;
} dba_dbm_data;

DBA_OPEN_FUNC(dbm)
{
	int fd;
	int filemode = info->file_permission;

	if(info->mode == DBA_TRUNC) {
		char buf[MAXPATHLEN];

		/* dbm/ndbm original */
		TRUNC_IT(".pag", O_TRUNC);
		TRUNC_IT(".dir", O_TRUNC);
	}

	if(info->mode == DBA_CREAT) {
		char buf[MAXPATHLEN];

		TRUNC_IT(".pag", 0);
		TRUNC_IT(".dir", 0);
	}

	if(dbminit((char *) ZSTR_VAL(info->path)) == -1) {
		return FAILURE;
	}

	info->dbf = pemalloc(sizeof(dba_dbm_data), info->flags&DBA_PERSISTENT);
	memset(info->dbf, 0, sizeof(dba_dbm_data));
	return SUCCESS;
}

DBA_CLOSE_FUNC(dbm)
{
	pefree(info->dbf, info->flags&DBA_PERSISTENT);
	dbmclose();
}

DBA_FETCH_FUNC(dbm)
{
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	gval = fetch(gkey);
	if (gval.dptr) {
		return zend_string_init(gval.dptr, gval.dsize, /* persistent */ false);
	}
	return NULL;
}

DBA_UPDATE_FUNC(dbm)
{
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);

	if (mode == 1) { /* insert */
		gval = fetch(gkey);
		if (gval.dptr) {
			return FAILURE;
		}
	}

	gval.dptr = ZSTR_VAL(val);
	gval.dsize = ZSTR_LEN(val);

	return (store(gkey, gval) == -1 ? FAILURE : SUCCESS);
}

DBA_EXISTS_FUNC(dbm)
{
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);

	gval = fetch(gkey);
	if (gval.dptr) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(dbm)
{
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	return(delete(gkey) == -1 ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(dbm)
{
	dba_dbm_data *dba = info->dbf;
	datum gkey;
	zend_string *key = NULL;

	gkey = firstkey();
	if (gkey.dptr) {
		key = zend_string_init(gkey.dptr, gkey.dsize, /* persistent */ false);
		dba->nextkey = gkey;
	} else {
		dba->nextkey.dptr = NULL;
	}
	return key;
}

DBA_NEXTKEY_FUNC(dbm)
{
	dba_dbm_data *dba = info->dbf;
	datum gkey;
	zend_string *key = NULL;

	if (!dba->nextkey.dptr) { return NULL; }

	gkey = nextkey(dba->nextkey);
	if (gkey.dptr) {
		key = zend_string_init(gkey.dptr, gkey.dsize, /* persistent */ false);
		dba->nextkey = gkey;
	} else {
		dba->nextkey.dptr = NULL;
	}
	return key;
}

DBA_OPTIMIZE_FUNC(dbm)
{
	/* dummy */
	return SUCCESS;
}

DBA_SYNC_FUNC(dbm)
{
	return SUCCESS;
}

DBA_INFO_FUNC(dbm)
{
#if DBA_GDBM
	if (!strcmp(DBM_VERSION, "GDBM"))
	{
		return dba_info_gdbm(hnd, info);
	}
#endif
	return estrdup(DBM_VERSION);
}

#endif
