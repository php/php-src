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

#if DBA_GDBM
#include "php_gdbm.h"

#include <gdbm.h>

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
		return FAILURE;

	if(info->argc > 0) {
		convert_to_long_ex(info->argv[0]);
		filemode = (*info->argv[0])->value.lval;
	}

	dbf = gdbm_open(info->path, 0, gmode, filemode, NULL);
	
	if(dbf) {
		info->dbf = malloc(sizeof(dba_gdbm_data));
		memset(info->dbf, 0, sizeof(dba_gdbm_data));
		((dba_gdbm_data *) info->dbf)->dbf = dbf;
		return SUCCESS;
	}
	return FAILURE;
}

DBA_CLOSE_FUNC(gdbm)
{
	GDBM_DATA;
	
	if(dba->nextkey.dptr) free(dba->nextkey.dptr);
	gdbm_close(dba->dbf);
	free(dba);
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

	if(gdbm_store(dba->dbf, gkey, gval, 
				mode == 1 ? GDBM_INSERT : GDBM_REPLACE) == 0)
		return SUCCESS;
	printf("XXX %s\n", gdbm_strerror(gdbm_errno));
	return FAILURE;
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
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: sw=4 ts=4 tw=78 fdm=marker
 */
