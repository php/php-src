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
			return FAILURE;
	}

	if(info->argc > 0) {
		convert_to_long_ex(info->argv[0]);
		filemode = (*info->argv[0])->value.lval;
	}

	dbf = dbm_open(info->path, gmode, filemode);
	
	if(dbf) {
		pinfo->dbf = dbf;
		return SUCCESS;
	}
	return FAILURE;
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
#endif
