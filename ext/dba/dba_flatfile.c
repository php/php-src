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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef DBA_FLATFILE
#include "php_flatfile.h"

#include "libflatfile/flatfile.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

DBA_OPEN_FUNC(flatfile)
{
	info->dbf = pemalloc(sizeof(flatfile), info->flags&DBA_PERSISTENT);
	memset(info->dbf, 0, sizeof(flatfile));

	((flatfile*)info->dbf)->fp = info->fp;

	return SUCCESS;
}

DBA_CLOSE_FUNC(flatfile)
{
	flatfile *dba = info->dbf;

	if (dba->nextkey.dptr) {
		efree(dba->nextkey.dptr);
	}
	pefree(dba, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(flatfile)
{
	flatfile *dba = info->dbf;
	datum gval;
	datum gkey;
	zend_string *fetched_val = NULL;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);

	gval = flatfile_fetch(dba, gkey);
	if (gval.dptr) {
		fetched_val = zend_string_init(gval.dptr, gval.dsize, /* persistent */ false);
		efree(gval.dptr);
	}
	return fetched_val;
}

DBA_UPDATE_FUNC(flatfile)
{
	flatfile *dba = info->dbf;
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	gval.dptr = ZSTR_VAL(val);
	gval.dsize = ZSTR_LEN(val);

	switch(flatfile_store(dba, gkey, gval, mode==1 ? FLATFILE_INSERT : FLATFILE_REPLACE)) {
		case 0:
			return SUCCESS;
		case 1:
			return FAILURE;
		case -1:
			// TODO Check when this happens and confirm this can even happen
			php_error_docref(NULL, E_WARNING, "Operation not possible");
			return FAILURE;
		default:
			// TODO Convert this to an assertion failure
			php_error_docref(NULL, E_WARNING, "Unknown return value");
			return FAILURE;
	}
}

DBA_EXISTS_FUNC(flatfile)
{
	flatfile *dba = info->dbf;
	datum gval;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	gval = flatfile_fetch(dba, gkey);
	if (gval.dptr) {
		efree(gval.dptr);
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(flatfile)
{
	flatfile *dba = info->dbf;
	datum gkey;

	gkey.dptr = ZSTR_VAL(key);
	gkey.dsize = ZSTR_LEN(key);
	return(flatfile_delete(dba, gkey) == -1 ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(flatfile)
{
	flatfile *dba = info->dbf;

	if (dba->nextkey.dptr) {
		efree(dba->nextkey.dptr);
	}
	dba->nextkey = flatfile_firstkey(dba);
	if (dba->nextkey.dptr) {
		return zend_string_init(dba->nextkey.dptr, dba->nextkey.dsize, /* persistent */ false);
	}
	return NULL;
}

DBA_NEXTKEY_FUNC(flatfile)
{
	flatfile *dba = info->dbf;

	if (!dba->nextkey.dptr) {
		return NULL;
	}

	if (dba->nextkey.dptr) {
		efree(dba->nextkey.dptr);
	}
	dba->nextkey = flatfile_nextkey(dba);
	if (dba->nextkey.dptr) {
		return zend_string_init(dba->nextkey.dptr, dba->nextkey.dsize, /* persistent */ false);
	}
	return NULL;
}

DBA_OPTIMIZE_FUNC(flatfile)
{
	/* dummy */
	return SUCCESS;
}

DBA_SYNC_FUNC(flatfile)
{
	/* dummy */
	return SUCCESS;
}

DBA_INFO_FUNC(flatfile)
{
	return estrdup(flatfile_version());
}

#endif
