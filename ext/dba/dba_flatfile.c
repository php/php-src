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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_FLATFILE
#include "php_flatfile.h"

#include "libflatfile/flatfile.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_31
#include "os/nt/flock.h"
#else
#ifdef PHP_WIN32
#include "win32/flock.h"
#else
#include <sys/file.h>
#endif
#endif

#define DBM_DATA flatfile *dba = info->dbf
#define DBM_GKEY datum gkey; gkey.dptr = (char *) key; gkey.dsize = keylen

#define TRUNC_IT(extension, mode) \
	snprintf(buf, MAXPATHLEN, "%s" extension, info->path); \
	buf[MAXPATHLEN-1] = '\0'; \
	if((fd = VCWD_OPEN_MODE(buf, O_CREAT | mode | O_WRONLY, filemode)) == -1) \
		return FAILURE; \
	close(fd);


DBA_OPEN_FUNC(flatfile)
{
	char *fmode;
	php_stream *fp;
	int lock;
	char *lockfn = NULL;
	int lockfd = 0;
#if NFS_HACK
	int last_try = 0;
	struct stat sb;
	int retries = 0;
#endif

	info->dbf = ecalloc(sizeof(flatfile), 1);
	if (!info->dbf) {
		*error = "Out of memory";
		return FAILURE;
	}

	switch(info->mode) {
		case DBA_READER:
			fmode = "r";
			lock = 0;
			break;
		case DBA_WRITER:
			fmode = "r+b";
			lock = 1;
			break;
		case DBA_CREAT:
			fmode = "a+b";
			lock = 1;
			break;
		case DBA_TRUNC:
			fmode = "w+b";
			lock = 1;
			break;
		default:
			efree(info->dbf);
			return FAILURE; /* not possible */
	}

	if (lock) {
		spprintf(&lockfn, 0, "%s.lck", info->path);

#if NFS_HACK      
		while((last_try = VCWD_STAT(lockfn, &sb))==0) {
			retries++;
			php_sleep(1);
			if (retries>30) 
				break;
		}	
		if (last_try!=0) {
			lockfd = open(lockfn, O_RDWR|O_CREAT, 0644);
			close(lockfd);
		} else {
			*error = "File appears to be locked";
			efree(lockfn);
			efree(info->dbf);
			return FAILURE;
		}
#else /* NFS_HACK */
		lockfd = VCWD_OPEN_MODE(lockfn, O_RDWR|O_CREAT, 0644);

		if (!lockfd || flock(lockfd, LOCK_EX)) {
			if (lockfd)
				close(lockfd);
			efree(lockfn);
			efree(info->dbf);
			*error = "Unable to establish lock";
			return FAILURE;
		}
#endif /* else NFS_HACK */
	}

	fp = php_stream_open_wrapper(info->path, fmode, STREAM_MUST_SEEK|IGNORE_PATH|ENFORCE_SAFE_MODE, NULL);
	if (!fp) {
		*error = "Unable to open file";
#if NFS_HACK
		VCWD_UNLINK(lockfn);
#else
		if (lockfn) {
			lockfd = VCWD_OPEN_MODE(lockfn, O_RDWR, 0644);
			flock(lockfd, LOCK_UN);
			close(lockfd);
		}
#endif
		efree(lockfn);
		efree(info->dbf);
		return FAILURE;
	}

	((flatfile*)info->dbf)->fp = fp;
	((flatfile*)info->dbf)->lockfn = lockfn;
	((flatfile*)info->dbf)->lockfd = lockfd;

	return SUCCESS;
}

DBA_CLOSE_FUNC(flatfile)
{
	DBM_DATA;

#if NFS_HACK
	VCWD_UNLINK(dba->lockfn);
#else
	if (dba->lockfn) {
		/*dba->lockfd = VCWD_OPEN_MODE(dba->lockfn, O_RDWR, 0644);*/
		flock(dba->lockfd, LOCK_UN);
		close(dba->lockfd);
	}
#endif
	efree(dba->lockfn);

	php_stream_close(dba->fp);
	if (dba->nextkey.dptr)
		efree(dba->nextkey.dptr);
	efree(dba);
}

#define DBM_FETCH(gkey)       dbm_file_fetch((flatfile*)info->dbf, gkey TSRMLS_CC)
#define DBM_STORE(gkey, gval) dbm_file_store((flatfile*)info->dbf, gkey, gval, DBM_REPLACE TSRMLS_CC)
#define DBM_DELETE(gkey)      dbm_file_delete((flatfile*)info->dbf, gkey TSRMLS_CC)
#define DBM_FIRSTKEY()        dbm_file_firstkey((flatfile*)info->dbf TSRMLS_CC)
#define DBM_NEXTKEY(gkey)     dbm_file_nextkey((flatfile*)info->dbf TSRMLS_CC)

DBA_FETCH_FUNC(flatfile)
{
	datum gval;
	char *new = NULL;

	DBM_GKEY;
	gval = DBM_FETCH(gkey);
	if(gval.dptr) {
		if(newlen) *newlen = gval.dsize;
		new = estrndup(gval.dptr, gval.dsize);
		efree(gval.dptr);
	}
	return new;
}

DBA_UPDATE_FUNC(flatfile)
{
	datum gval;

	DBM_GKEY;
	gval.dptr = (char *) val;
	gval.dsize = vallen;
	
	return (DBM_STORE(gkey, gval) == -1 ? FAILURE : SUCCESS);
}

DBA_EXISTS_FUNC(flatfile)
{
	datum gval;
	DBM_GKEY;
	
	gval = DBM_FETCH(gkey);
	if(gval.dptr) {
		efree(gval.dptr);
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(flatfile)
{
	DBM_GKEY;
	return(DBM_DELETE(gkey) == -1 ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(flatfile)
{
	DBM_DATA;

	if (dba->nextkey.dptr)
		efree(dba->nextkey.dptr);
	dba->nextkey = DBM_FIRSTKEY();
	if(dba->nextkey.dptr) {
		if(newlen) 
			*newlen = dba->nextkey.dsize;
		return estrndup(dba->nextkey.dptr, dba->nextkey.dsize);
	}
	return NULL;
}

DBA_NEXTKEY_FUNC(flatfile)
{
	DBM_DATA;
	datum lkey;
	
	if(!dba->nextkey.dptr) 
		return NULL;
	
	lkey = dba->nextkey;
	dba->nextkey = DBM_NEXTKEY(lkey);
	if (lkey.dptr)
		efree(lkey.dptr);
	if(dba->nextkey.dptr) {
		if(newlen) 
			*newlen = dba->nextkey.dsize;
		return estrndup(dba->nextkey.dptr, dba->nextkey.dsize);
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

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
