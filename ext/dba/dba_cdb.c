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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_CDB
#include "php_cdb.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <cdb.h>
#include <uint32.h>

#define CDB_INFO \
	dba_cdb *cdb = (dba_cdb *) info->dbf

typedef struct {
	struct cdb c;
	int fd;
	uint32 eod; /* size of constant database */
	uint32 pos; /* current position for traversing */
} dba_cdb;

DBA_OPEN_FUNC(cdb)
{
	int gmode = 0;
	int fd;
	dba_cdb *cdb;
	dba_info *pinfo = (dba_info *) info;

	switch (info->mode) {
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

	fd = VCWD_OPEN(info->path, gmode);
	if (fd < 0) {
		return FAILURE;
	}
	
	cdb = malloc(sizeof *cdb);
	memset(cdb, 0, sizeof *cdb);

	cdb_init(&cdb->c, fd);
	cdb->fd = fd;
	
	pinfo->dbf = cdb;
	return SUCCESS;
}

DBA_CLOSE_FUNC(cdb)
{
	CDB_INFO;

	/* cdb_free does not close associated fd */
	cdb_free(&cdb->c);
	close(cdb->fd);
	free(cdb);
}

DBA_FETCH_FUNC(cdb)
{
	CDB_INFO;
	unsigned int len;
	char *new_entry = NULL;
	
	if (cdb_find(&cdb->c, key, keylen) == 1) {
		len = cdb_datalen(&cdb->c);
		new_entry = emalloc(len);
		
		if (cdb_read(&cdb->c, new_entry, len, cdb_datapos(&cdb->c)) == -1) {
			free(new_entry);
			return NULL;
		}
		
		if (newlen) 
			*newlen = len;
	}
	
	return new_entry;
}

DBA_UPDATE_FUNC(cdb)
{
	/* if anyone figures out cdbmake.c, let me know */
	return FAILURE;
}

DBA_EXISTS_FUNC(cdb)
{
	CDB_INFO;

	if (cdb_find(&cdb->c, key, keylen) == 1)
		return SUCCESS;
	return FAILURE;
}

DBA_DELETE_FUNC(cdb)
{
	return FAILURE;
}


#define CREAD(n) do { \
	if (read(cdb->fd, buf, n) < n) return NULL; \
} while (0)

#define CSEEK(n) do { \
	if (n >= cdb->eod) return NULL; \
	if (lseek(cdb->fd, (off_t)n, SEEK_SET) != (off_t) n) return NULL; \
} while (0)


DBA_FIRSTKEY_FUNC(cdb)
{
	CDB_INFO;
	uint32 klen, dlen;
	char buf[8];
	char *key;

	cdb->eod = -1;
	CSEEK(0);
	CREAD(4);
	
	/* Total length of file in bytes */
	uint32_unpack(buf, &cdb->eod);
	
	CSEEK(2048);
	CREAD(8);
	
	/* The first four bytes contain the length of the key */
	uint32_unpack(buf, &klen);
	uint32_unpack(buf + 4, &dlen);

	key = emalloc(klen + 1);
	if (read(cdb->fd, key, klen) < klen) {
		efree(key);
		key = NULL;
	} else {
		key[klen] = '\0';
		if (newlen) *newlen = klen;
	}

	/*       header + klenlen + dlenlen + klen + dlen */
	cdb->pos = 2048 + 4       + 4       + klen + dlen;
		
	return key;
}

DBA_NEXTKEY_FUNC(cdb)
{
	CDB_INFO;
	uint32 klen, dlen;
	char buf[8];
	char *key;

	CSEEK(cdb->pos);
	CREAD(8);
	uint32_unpack(buf, &klen);
	uint32_unpack(buf + 4, &dlen);
	
	key = emalloc(klen + 1);
	if (read(cdb->fd, key, klen) < klen) {
		efree(key);
		key = NULL;
	} else {
		key[klen] = '\0';
		if (newlen) *newlen = klen;
	}
	
	cdb->pos += 8 + klen + dlen;
	
	return key;
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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
