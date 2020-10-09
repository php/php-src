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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_CDB
#include "php_cdb.h"

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#if DBA_CDB_BUILTIN
# include "libcdb/cdb.h"
# include "libcdb/cdb_make.h"
# include "libcdb/uint32.h"
#else
# ifdef CDB_INCLUDE_FILE
#  include CDB_INCLUDE_FILE
# endif
#endif

#define CDB_INFO \
	dba_cdb *cdb = (dba_cdb *) info->dbf

typedef struct {
	struct cdb c;
#if DBA_CDB_BUILTIN
	struct cdb_make m;
	php_stream *file;
	int make;
#else
	int file;
#endif
	uint32 eod; /* size of constant database */
	uint32 pos; /* current position for traversing */
} dba_cdb;

DBA_OPEN_FUNC(cdb)
{
#if DBA_CDB_BUILTIN
	php_stream* file = 0;
	int make;
#else
	int file = 0;
#endif
	dba_cdb *cdb;
	dba_info *pinfo = (dba_info *) info;

	switch (info->mode) {
		case DBA_READER:
#if DBA_CDB_BUILTIN
			make = 0;
			file = info->fp;
#else
			file = VCWD_OPEN(info->path, O_RDONLY);
			if (file < 0) {
				*error = "Unable to open file";
				return FAILURE;
			}
#endif
			break;
#if DBA_CDB_BUILTIN
		case DBA_TRUNC:
			make = 1;
			file = info->fp;
			break;
		case DBA_CREAT:
		case DBA_WRITER:
			*error = "Update operations are not supported";
			return FAILURE; /* not supported */
#endif
		default:
			*error = "Currently not supported";
			return FAILURE;
	}

	cdb = pemalloc(sizeof(dba_cdb), info->flags&DBA_PERSISTENT);
	memset(cdb, 0, sizeof(dba_cdb));

#if DBA_CDB_BUILTIN
	if (make) {
		cdb_make_start(&cdb->m, file);
	} else {
		cdb_init(&cdb->c, file);
	}
	cdb->make = make;
#else
	cdb_init(&cdb->c, file);
#endif
	cdb->file = file;

	pinfo->dbf = cdb;
	return SUCCESS;
}

DBA_CLOSE_FUNC(cdb)
{
	CDB_INFO;

	/* cdb_free does not close associated file */
#if DBA_CDB_BUILTIN
	if (cdb->make) {
		cdb_make_finish(&cdb->m);
	} else {
		cdb_free(&cdb->c);
	}
#else
	cdb_free(&cdb->c);
	close(cdb->file);
#endif
	pefree(cdb, info->flags&DBA_PERSISTENT);
}

#if DBA_CDB_BUILTIN
# define php_cdb_read(cdb, buf, len, pos) cdb_read(cdb, buf, len, pos)
# define php_cdb_findnext(cdb, key, len) cdb_findnext(cdb, key, len)
# define php_cdb_find(cdb, key, len) cdb_find(cdb, key, len)
#else
# define php_cdb_read(cdb, buf, len, pos) cdb_read(cdb, buf, len, pos)
# define php_cdb_findnext(cdb, key, len) cdb_findnext(cdb, key, len)
# define php_cdb_find(cdb, key, len) cdb_find(cdb, key, len)
#endif

DBA_FETCH_FUNC(cdb)
{
	CDB_INFO;
	unsigned int len;
	char *new_entry = NULL;

#if DBA_CDB_BUILTIN
	if (cdb->make)
		return NULL; /* database was opened writeonly */
#endif
	if (php_cdb_find(&cdb->c, key, keylen) == 1) {
		while(skip--) {
			if (php_cdb_findnext(&cdb->c, key, keylen) != 1) {
				return NULL;
			}
		}
		len = cdb_datalen(&cdb->c);
		new_entry = safe_emalloc(len, 1, 1);

		if (php_cdb_read(&cdb->c, new_entry, len, cdb_datapos(&cdb->c)) == -1) {
			efree(new_entry);
			return NULL;
		}
		new_entry[len] = 0;
		if (newlen)
			*newlen = len;
	}

	return new_entry;
}

DBA_UPDATE_FUNC(cdb)
{
#if DBA_CDB_BUILTIN
	CDB_INFO;

	if (!cdb->make)
		return FAILURE; /* database was opened readonly */
	if (!mode)
		return FAILURE; /* cdb_make doesn't know replace */
	if (cdb_make_add(&cdb->m, key, keylen, val, vallen) != -1)
		return SUCCESS;
#endif
	return FAILURE;
}

DBA_EXISTS_FUNC(cdb)
{
	CDB_INFO;

#if DBA_CDB_BUILTIN
	if (cdb->make)
		return FAILURE; /* database was opened writeonly */
#endif
	if (php_cdb_find(&cdb->c, key, keylen) == 1)
		return SUCCESS;
	return FAILURE;
}

DBA_DELETE_FUNC(cdb)
{
	return FAILURE; /* cdb doesn't support delete */
}

/* {{{ cdb_file_read */
#if DBA_CDB_BUILTIN
# define cdb_file_read(fildes, buf, size) php_stream_read(fildes, buf, size)
#else
# define cdb_file_read(fildes, buf, size) read(fildes, buf, size)
#endif
/* }}} */

#define CREAD(n) do { \
	if (cdb_file_read(cdb->file, buf, n) < n) return NULL; \
} while (0)

/* {{{ cdb_file_lseek
 php_stream_seek does not return actual position */
#if DBA_CDB_BUILTIN
zend_off_t cdb_file_lseek(php_stream *fp, zend_off_t offset, int whence) {
	php_stream_seek(fp, offset, whence);
	return php_stream_tell(fp);
}
#else
zend_off_t cdb_file_lseek(int fd, zend_off_t offset, int whence) {
	return lseek(fd, offset, whence);
}
#endif
/* }}} */

#define CSEEK(n) do { \
	if (n >= cdb->eod) return NULL; \
	if (cdb_file_lseek(cdb->file, (zend_off_t)n, SEEK_SET) != (zend_off_t) n) return NULL; \
} while (0)


DBA_FIRSTKEY_FUNC(cdb)
{
	CDB_INFO;
	uint32 klen, dlen;
	char buf[8];
	char *key;

#if DBA_CDB_BUILTIN
	if (cdb->make)
		return NULL; /* database was opened writeonly */
#endif

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

	key = safe_emalloc(klen, 1, 1);
	if (cdb_file_read(cdb->file, key, klen) < klen) {
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

#if DBA_CDB_BUILTIN
	if (cdb->make)
		return NULL; /* database was opened writeonly */
#endif

	CSEEK(cdb->pos);
	CREAD(8);
	uint32_unpack(buf, &klen);
	uint32_unpack(buf + 4, &dlen);

	key = safe_emalloc(klen, 1, 1);
	if (cdb_file_read(cdb->file, key, klen) < klen) {
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

DBA_INFO_FUNC(cdb)
{
#if DBA_CDB_BUILTIN
	if (!strcmp(hnd->name, "cdb")) {
		return estrdup(cdb_version());
	} else {
		return estrdup(cdb_make_version());
	}
#else
	return estrdup("External");
#endif
}

#endif
