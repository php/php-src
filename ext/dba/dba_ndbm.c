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

#ifdef DBA_NDBM
#include "php_ndbm.h"

#include <fcntl.h>
#ifdef NDBM_INCLUDE_FILE
#include NDBM_INCLUDE_FILE
#endif

DBA_OPEN_FUNC(ndbm)
{
	DBM *dbf;
	int gmode = 0;
	int filemode = info->file_permission;
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

	dbf = dbm_open(ZSTR_VAL(info->path), gmode, filemode);

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
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	gval = dbm_fetch(info->dbf, gkey);
	if (gval.dptr) {
		return zend_string_init(gval.dptr, gval.dsize, /* persistent */ false);
	}
	return NULL;
}

DBA_UPDATE_FUNC(ndbm)
{
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	gval.dptr = ZSTR_VAL(val);
	gval.dsize = ZSTR_LEN(val);

	if(!dbm_store(info->dbf, gkey, gval, mode == 1 ? DBM_INSERT : DBM_REPLACE))
		return SUCCESS;
	return FAILURE;
}

DBA_EXISTS_FUNC(ndbm)
{
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	gval = dbm_fetch(info->dbf, gkey);
	if (gval.dptr) {
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(ndbm)
{
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	return(dbm_delete(info->dbf, gkey) == -1 ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(ndbm)
{
	datum gkey;

	gkey = dbm_firstkey(info->dbf);
	if (gkey.dptr) {
		return zend_string_init(gkey.dptr, gkey.dsize, /* persistent */ false);
	}
	return NULL;
}

DBA_NEXTKEY_FUNC(ndbm)
{
	datum gkey;

	gkey = dbm_nextkey(info->dbf);
	if (gkey.dptr) {
		return zend_string_init(gkey.dptr, gkey.dsize, /* persistent */ false);
	}
	return NULL;
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
