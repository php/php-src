/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_DBM
#include "php_dbm.h"

#ifdef DBA_DBM_BUILTIN
#include "libdbm/dbm.h"
#else
#include <dbm.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef DBA_DBM_BUILTIN
typedef struct {
	datum nextkey;
} dba_dbm_data;
#endif

#define DBM_DATA dba_dbm_data *dba = info->dbf
#define DBM_GKEY datum gkey; gkey.dptr = (char *) key; gkey.dsize = keylen

#define TRUNC_IT(extension, mode) \
	snprintf(buf, MAXPATHLEN, "%s" extension, info->path); \
	buf[MAXPATHLEN-1] = '\0'; \
	if((fd = VCWD_OPEN_MODE(buf, O_CREAT | mode | O_WRONLY, filemode)) == -1) \
		return FAILURE; \
	close(fd);


DBA_OPEN_FUNC(dbm)
{
#ifdef DBA_DBM_BUILTIN
	char *fmode;
	php_stream *fp;

	info->dbf = ecalloc(sizeof(dba_dbm_data), 1);
	if (!info->dbf) {
		*error = "Out of memory";
		return FAILURE;
	}

	switch(info->mode) {
		case DBA_READER:
			fmode = "r";
			break;
		case DBA_WRITER:
			fmode = "a+";
			break;
		case DBA_CREAT:
		case DBA_TRUNC:
			fmode = "w+";
			break;
		default:
			return FAILURE; /* not possible */
	}
	fp = php_stream_open_wrapper(info->path, fmode, STREAM_MUST_SEEK|IGNORE_PATH|ENFORCE_SAFE_MODE, NULL);
	if (!fp) {
		*error = "Unable to open file";
		return FAILURE;
	}

	((dba_dbm_data*)info->dbf)->fp = fp;
	return SUCCESS;
#else
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

	info->dbf = ecalloc(sizeof(dba_dbm_data), 1);
	if (!info->dbf) {
		*error = "Out of memory";
		return FAILURE;
	}
	return SUCCESS;
#endif
}

DBA_CLOSE_FUNC(dbm)
{
	DBM_DATA;

#ifdef DBA_DBM_BUILTIN
	php_stream_close(dba->fp);
	if (dba->nextkey.dptr)
		efree(dba->nextkey.dptr);
#else
	dbmclose();
#endif
	efree(dba);
}

#ifdef DBA_DBM_BUILTIN
#define DBM_FETCH(gkey)       dbm_file_fetch((dba_dbm_data*)info->dbf, gkey TSRMLS_CC)
#define DBM_STORE(gkey, gval) dbm_file_store((dba_dbm_data*)info->dbf, gkey, gval, DBM_REPLACE TSRMLS_CC)
#define DBM_DELETE(gkey)      dbm_file_delete((dba_dbm_data*)info->dbf, gkey TSRMLS_CC)
#define DBM_FIRSTKEY()        dbm_file_firstkey((dba_dbm_data*)info->dbf TSRMLS_CC)
#define DBM_NEXTKEY(gkey)     dbm_file_nextkey((dba_dbm_data*)info->dbf TSRMLS_CC)
#else
#define DBM_FETCH(gkey)       fetch(gkey)
#define DBM_STORE(gkey, gval) store(gkey, gval)
#define DBM_DELETE(gkey)      delete(gkey)
#define DBM_FIRSTKEY()        firstkey()
#define DBM_NEXTKEY(gkey)     nextkey(gkey)
#endif

DBA_FETCH_FUNC(dbm)
{
	datum gval;
	char *new = NULL;

	DBM_GKEY;
	gval = DBM_FETCH(gkey);
	if(gval.dptr) {
		if(newlen) *newlen = gval.dsize;
		new = estrndup(gval.dptr, gval.dsize);
#ifdef DBA_DBM_BUILTIN
		efree(gval.dptr);
#endif
	}
	return new;
}

DBA_UPDATE_FUNC(dbm)
{
	datum gval;

	DBM_GKEY;
	gval.dptr = (char *) val;
	gval.dsize = vallen;
	
	return (DBM_STORE(gkey, gval) == -1 ? FAILURE : SUCCESS);
}

DBA_EXISTS_FUNC(dbm)
{
	datum gval;
	DBM_GKEY;
	
	gval = DBM_FETCH(gkey);
	if(gval.dptr) {
#ifdef DBA_DBM_BUILTIN
		efree(gval.dptr);
#endif
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(dbm)
{
	DBM_GKEY;
	return(DBM_DELETE(gkey) == -1 ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(dbm)
{
	DBM_DATA;

#ifdef DBA_DBM_BUILTIN
	if (dba->nextkey.dptr)
		efree(dba->nextkey.dptr);
#endif
	dba->nextkey = DBM_FIRSTKEY();
	if(dba->nextkey.dptr) {
		if(newlen) 
			*newlen = dba->nextkey.dsize;
		return estrndup(dba->nextkey.dptr, dba->nextkey.dsize);
	}
	return NULL;
}

DBA_NEXTKEY_FUNC(dbm)
{
	DBM_DATA;
	datum lkey;
	
	if(!dba->nextkey.dptr) 
		return NULL;
	
	lkey = dba->nextkey;
	dba->nextkey = DBM_NEXTKEY(lkey);
#ifdef DBA_DBM_BUILTIN
	if (lkey.dptr)
		efree(lkey.dptr);
#endif
	if(dba->nextkey.dptr) {
		if(newlen) 
			*newlen = dba->nextkey.dsize;
		return estrndup(dba->nextkey.dptr, dba->nextkey.dsize);
	}
	return NULL;
}

DBA_OPTIMIZE_FUNC(dbm)
{
	/* dummy */
	return SUCCESS;
}

DBA_SYNC_FUNC(dbm)
{
	/* dummy */
	return SUCCESS;
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
