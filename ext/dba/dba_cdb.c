/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#if DBA_CDB
#include "php_cdb.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <cdb.h>
#include <cdbmake.h>

#define CDB_INFO \
	dba_cdb *cdb = (dba_cdb *) info->dbf

typedef struct {
	int fd;
	uint32 eod; /* size of constant database */
	uint32 pos; /* current position for traversing */
} dba_cdb;

DBA_OPEN_FUNC(cdb)
{
	int gmode = 0;
	dba_cdb *cdb;
	dba_info *pinfo = (dba_info *) info;

	switch(info->mode) {
		case DBA_READER: 
			gmode = O_RDONLY; break;
		/* currently not supported: */
#if 0
		case DBA_WRITER: 
			gmode = O_RDWR; break;
#endif
		default: 
			return FAILURE;
	}

	cdb = malloc(sizeof *cdb);
	memset(cdb, 0, sizeof *cdb);

	cdb->fd = VCWD_OPEN((info->path, gmode));
	if(cdb->fd < 0) {
		free(cdb);
		return FAILURE;
	}
	
	pinfo->dbf = cdb;
	return SUCCESS;
}

DBA_CLOSE_FUNC(cdb)
{
	CDB_INFO;

	close(cdb->fd);
	free(cdb);
}

DBA_FETCH_FUNC(cdb)
{
	CDB_INFO;
	int len;
	char *new = NULL;
	
	if(cdb_seek(cdb->fd, key, keylen, &len) == 1) {
		new = emalloc(len);
		read(cdb->fd, new, len);
		if(newlen) *newlen = len;
	}
	
	return new;
}

DBA_UPDATE_FUNC(cdb)
{
	/* if anyone figures out cdbmake.c, let me know */
	return FAILURE;
}

DBA_EXISTS_FUNC(cdb)
{
	CDB_INFO;
	int len;

	if(cdb_seek(cdb->fd, key, keylen, &len) == 1)
		return SUCCESS;
	return FAILURE;
}

DBA_DELETE_FUNC(cdb)
{
	return FAILURE;
}


#define CREAD(n) if(read(cdb->fd, buf, n) < n) return NULL
#define CSEEK(n) \
	if(n >= cdb->eod) return NULL; \
	if(lseek(cdb->fd, (off_t)n, SEEK_SET) != (off_t) n) return NULL

DBA_FIRSTKEY_FUNC(cdb)
{
	CDB_INFO;
	uint32 len;
	char buf[8];
	char *key;

	cdb->eod = -1;
	CSEEK(0);
	CREAD(4);
	cdb->eod = cdb_unpack(buf);
	
	CSEEK(2048);
	CREAD(8);
	len = cdb_unpack(buf);

	key = emalloc(len + 1);
	if(read(cdb->fd, key, len) < len) {
		efree(key);
		key = NULL;
	} else
		key[len] = '\0';
	/*       header + klenlen + dlenlen + klen + dlen */
	cdb->pos = 2048 + 4       + 4       + len  + cdb_unpack(buf + 4);
		
	return key;
}

DBA_NEXTKEY_FUNC(cdb)
{
	CDB_INFO;
	uint32 len;
	char buf[8];
	char *nkey;

	CSEEK(cdb->pos);
	CREAD(8);
	len = cdb_unpack(buf);
	
	nkey = emalloc(len + 1);
	if(read(cdb->fd, nkey, len) < len) {
		efree(nkey);
		return NULL;
	}
	nkey[len] = '\0';
	if(newlen) *newlen = len;
	
	cdb->pos += 8 + len + cdb_unpack(buf + 4);
	
	return nkey;
#if 0
	/* this code cdb_seeks and is thus slower than directly seeking
	   in the file */
	CDB_INFO;
	char *nkey = NULL;
	uint32 len;
	char buf[8];
	
	if(cdb_seek(cdb->fd, key, keylen, &len) == 1) {
		if(lseek(cdb->fd, (off_t) len, SEEK_CUR) >= (off_t) cdb->eod) 
			return NULL;
		CREAD(8);
		len = cdb_unpack(buf);

		nkey = emalloc(len + 1);
		if(read(cdb->fd, nkey, len) < len) {
			efree(nkey);
			nkey = NULL;
		} else
			nkey[len] = '\0';
	}
	return nkey;
#endif
}

DBA_OPTIMIZE_FUNC(cdb)
{
	return SUCCESS;
}

DBA_SYNC_FUNC(cdb)
{
	/* this is read-only */
	return SUCCESS;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
