/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999 PHP Development Team (See Credits file)           |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#if DBA_DBM
#include "php_dbm.h"

#include <dbm.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DBM_DATA dba_dbm_data *dba = info->dbf
#define DBM_GKEY datum gkey; gkey.dptr = (char *) key; gkey.dsize = keylen

#define TRUNC_IT(extension, mode) \
	snprintf(buf, MAXPATHLEN, "%s" extension, info->path); \
	buf[MAXPATHLEN-1] = '\0'; \
	if((fd = VCWD_OPEN((buf, O_CREAT | mode | O_WRONLY, filemode))) == -1) \
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
		filemode = (*info->argv[0])->value.lval;
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

	info->dbf = calloc(sizeof(dba_dbm_data), 1);
	return SUCCESS;
}

DBA_CLOSE_FUNC(dbm)
{
	free(info->dbf);
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

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: sw=4 ts=4 tw=78 fdm=marker
 */
