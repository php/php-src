/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:  Michael Maclean <mgdm@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_TCADB
#include "php_tcadb.h"

#ifdef TCADB_INCLUDE_FILE
#include TCADB_INCLUDE_FILE
#endif

#define TCADB_DATA dba_tcadb_data *dba = info->dbf

typedef struct {
	TCADB *tcadb;	
} dba_tcadb_data;

DBA_OPEN_FUNC(tcadb)
{
	char *path_string;
	TCADB *tcadb = tcadbnew();

	if (tcadb) {
		switch(info->mode) {
			case DBA_READER:
				spprintf(&path_string, 0, "%s#mode=r", info->path);
				break;
			case DBA_WRITER:
				spprintf(&path_string, 0, "%s#mode=w", info->path);
				break;
			case DBA_CREAT:
				spprintf(&path_string, 0, "%s#mode=wc", info->path);
				break;
			case DBA_TRUNC:
				spprintf(&path_string, 0, "%s#mode=wct", info->path);
				break;
			default:
				tcadbdel(tcadb);
				return FAILURE;
		}

		if (!tcadbopen(tcadb, path_string)) {
			efree(path_string);
			tcadbdel(tcadb);
			return FAILURE;
		}

		efree(path_string);

		info->dbf = pemalloc(sizeof(dba_tcadb_data), info->flags & DBA_PERSISTENT);
		memset(info->dbf, 0, sizeof(dba_tcadb_data));
		((dba_tcadb_data *) info->dbf)->tcadb = tcadb;
		return SUCCESS;
	}

	return FAILURE;
}

DBA_CLOSE_FUNC(tcadb)
{
	TCADB_DATA;

	tcadbclose(dba->tcadb);
	pefree(dba, info->flags & DBA_PERSISTENT);
}

DBA_FETCH_FUNC(tcadb)
{
	TCADB_DATA;
	char *value, *new = NULL;
	int value_size;

	value = tcadbget(dba->tcadb, key, keylen, &value_size);
	if (value) {
		if (newlen) {
			*newlen = value_size;
		}
		new = estrndup(value, value_size);
		tcfree(value);
	}

	return new;
}

DBA_UPDATE_FUNC(tcadb)
{
	TCADB_DATA;
	int result;

	if (mode == 1) {
		/* Insert */
		if (tcadbvsiz(dba->tcadb, key, keylen) > -1) {
			return FAILURE;
		}
	}
	
	result = tcadbput(dba->tcadb, key, keylen, val, vallen);

	if (result) {
		return SUCCESS;
	}

	php_error_docref2(NULL TSRMLS_CC, key, val, E_WARNING, "Error updating data");
	return FAILURE;
}

DBA_EXISTS_FUNC(tcadb)
{
	TCADB_DATA;
	char *value;
	int value_len;

	value = tcadbget(dba->tcadb, key, keylen, &value_len);
	if (value) {
		tcfree(value);
		return SUCCESS;
	}

	return FAILURE;
}

DBA_DELETE_FUNC(tcadb)
{
	TCADB_DATA;

	return tcadbout(dba->tcadb, key, keylen) ? SUCCESS : FAILURE;
}

DBA_FIRSTKEY_FUNC(tcadb)
{
	TCADB_DATA;
	int value_size;
	char *value, *new = NULL;

	tcadbiterinit(dba->tcadb);

	value = tcadbiternext(dba->tcadb, &value_size);
	if (value) {
		if (newlen) {
			*newlen = value_size;
		}
		new = estrndup(value, value_size);
		tcfree(value);
	}

	return new;
}

DBA_NEXTKEY_FUNC(tcadb)
{
	TCADB_DATA;
	int value_size;
	char *value, *new = NULL;

	value = tcadbiternext(dba->tcadb, &value_size);
	if (value) {
		if (newlen) {
			*newlen = value_size;
		}
		new = estrndup(value, value_size);
		tcfree(value);
	}

	return new;
}

DBA_OPTIMIZE_FUNC(tcadb)
{
	TCADB_DATA;

#if _TC_LIBVER >= 811
	return tcadboptimize(dba->tcadb, NULL) ? SUCCESS : FAILURE;
#else
	return FAILURE;
#endif
}

DBA_SYNC_FUNC(tcadb)
{
	TCADB_DATA;

	return tcadbsync(dba->tcadb) ? SUCCESS : FAILURE;
}

DBA_INFO_FUNC(tcadb)
{
	return estrdup(tcversion);
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
