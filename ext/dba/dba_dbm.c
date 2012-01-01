/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_DBM
#include "php_dbm.h"

#ifdef DBM_INCLUDE_FILE
#include DBM_INCLUDE_FILE
#endif
#if DBA_GDBM
#include "php_gdbm.h"
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DBM_DATA dba_dbm_data *dba = info->dbf
#define DBM_GKEY datum gkey; gkey.dptr = (char *) key; gkey.dsize = keylen

#define TRUNC_IT(extension, mode) \
	snprintf(buf, MAXPATHLEN, "%s" extension, info->path); \
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
	int filemode = 0644;

	if(info->argc > 0) {
		convert_to_long_ex(info->argv[0]);
		filemode = Z_LVAL_PP(info->argv[0]);
	}
	
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

	if(dbminit((char *) info->path) == -1) {
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
	char *new = NULL;

	DBM_GKEY;
	gval = fetch(gkey);
	if(gval.dptr) {
		if(newlen) *newlen = gval.dsize;
		new = estrndup(gval.dptr, gval.dsize);
	}
	return new;
}

DBA_UPDATE_FUNC(dbm)
{
	datum gval;

	DBM_GKEY;

	if (mode == 1) { /* insert */
		gval = fetch(gkey);
		if(gval.dptr) {
			return FAILURE;
		}
	}

	gval.dptr = (char *) val;
	gval.dsize = vallen;
	
	return (store(gkey, gval) == -1 ? FAILURE : SUCCESS);
}

DBA_EXISTS_FUNC(dbm)
{
	datum gval;
	DBM_GKEY;
	
	gval = fetch(gkey);
	if(gval.dptr) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(dbm)
{
	DBM_GKEY;
	return(delete(gkey) == -1 ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(dbm)
{
	DBM_DATA;
	datum gkey;
	char *key = NULL;

	gkey = firstkey();
	if(gkey.dptr) {
		if(newlen) *newlen = gkey.dsize;
		key = estrndup(gkey.dptr, gkey.dsize);
		dba->nextkey = gkey;
	} else
		dba->nextkey.dptr = NULL;
	return key;
}

DBA_NEXTKEY_FUNC(dbm)
{
	DBM_DATA;
	datum gkey;
	char *nkey = NULL;
	
	if(!dba->nextkey.dptr) return NULL;
	
	gkey = nextkey(dba->nextkey);
	if(gkey.dptr) {
		if(newlen) *newlen = gkey.dsize;
		nkey = estrndup(gkey.dptr, gkey.dsize);
		dba->nextkey = gkey;
	} else
		dba->nextkey.dptr = NULL;
	return nkey;
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
		return dba_info_gdbm(hnd, info TSRMLS_CC);
	}
#endif
	return estrdup(DBM_VERSION);
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
