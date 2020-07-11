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

#if DBA_NDBM
#include "php_ndbm.h"

#include <fcntl.h>
#ifdef NDBM_INCLUDE_FILE
#include NDBM_INCLUDE_FILE
#endif

#define NDBM_GKEY datum gkey; gkey.dptr = (char *) key; gkey.dsize = keylen

DBA_OPEN_FUNC(ndbm)
{
	DBM *dbf;
	int gmode = 0;
	int filemode = 0644;
	dba_info *pinfo = (dba_info *) info;

	switch(info->mode) {
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

	if(info->argc > 0) {
		filemode = zval_get_long(&info->argv[0]);
	}

	dbf = dbm_open(info->path, gmode, filemode);

	pinfo->dbf = dbf;
	return SUCCESS;
}

DBA_CLOSE_FUNC(ndbm)
{
	dbm_close(info->dbf);
}

DBA_FETCH_FUNC(ndbm)
{
	datum gval;
	char *new = NULL;

	NDBM_GKEY;
	gval = dbm_fetch(info->dbf, gkey);
	if(gval.dptr) {
		if(newlen) *newlen = gval.dsize;
		new = estrndup(gval.dptr, gval.dsize);
	}
	return new;
}

DBA_UPDATE_FUNC(ndbm)
{
	datum gval;

	NDBM_GKEY;
	gval.dptr = (char *) val;
	gval.dsize = vallen;

	if(!dbm_store(info->dbf, gkey, gval, mode == 1 ? DBM_INSERT : DBM_REPLACE))
		return SUCCESS;
	return FAILURE;
}

DBA_EXISTS_FUNC(ndbm)
{
	datum gval;
	NDBM_GKEY;
	gval = dbm_fetch(info->dbf, gkey);
	if(gval.dptr) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(ndbm)
{
	NDBM_GKEY;
	return(dbm_delete(info->dbf, gkey) == -1 ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(ndbm)
{
	datum gkey;
	char *key = NULL;

	gkey = dbm_firstkey(info->dbf);
	if(gkey.dptr) {
		if(newlen) *newlen = gkey.dsize;
		key = estrndup(gkey.dptr, gkey.dsize);
	}
	return key;
}

DBA_NEXTKEY_FUNC(ndbm)
{
	datum gkey;
	char *nkey = NULL;

	gkey = dbm_nextkey(info->dbf);
	if(gkey.dptr) {
		if(newlen) *newlen = gkey.dsize;
		nkey = estrndup(gkey.dptr, gkey.dsize);
	}
	return nkey;
}

DBA_OPTIMIZE_FUNC(ndbm)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(ndbm)
{
	return SUCCESS;
}

DBA_INFO_FUNC(ndbm)
{
	return estrdup("NDBM");
}

#endif
