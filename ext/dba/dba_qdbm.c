/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Marcin Gibula <mg@iceni.pl>                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_QDBM
#include "php_qdbm.h"

#ifdef QDBM_INCLUDE_FILE
#include QDBM_INCLUDE_FILE
#endif

#define QDBM_DATA dba_qdbm_data *dba = info->dbf

typedef struct {
	DEPOT *dbf;
} dba_qdbm_data;

DBA_OPEN_FUNC(qdbm)
{
	DEPOT *dbf;

	switch(info->mode) {
		case DBA_READER:
			dbf = dpopen(info->path, DP_OREADER, 0);
			break;
		case DBA_WRITER:
			dbf = dpopen(info->path, DP_OWRITER, 0);
			break;
		case DBA_CREAT:
			dbf = dpopen(info->path, DP_OWRITER | DP_OCREAT, 0);
			break;
		case DBA_TRUNC:
			dbf = dpopen(info->path, DP_OWRITER | DP_OCREAT | DP_OTRUNC, 0);
			break;
		default:
			return FAILURE;
	}

	if (dbf) {
		info->dbf = pemalloc(sizeof(dba_qdbm_data), info->flags & DBA_PERSISTENT);
		memset(info->dbf, 0, sizeof(dba_qdbm_data));
		((dba_qdbm_data *) info->dbf)->dbf = dbf;
		return SUCCESS;
	}

	*error = (char *) dperrmsg(dpecode);
	return FAILURE;
}

DBA_CLOSE_FUNC(qdbm)
{
	QDBM_DATA;

	dpclose(dba->dbf);
	pefree(dba, info->flags & DBA_PERSISTENT);
}

DBA_FETCH_FUNC(qdbm)
{
	QDBM_DATA;
	char *value, *new = NULL;
	int value_size;

	value = dpget(dba->dbf, key, keylen, 0, -1, &value_size);
	if (value) {
		if (newlen) *newlen = value_size;
		new = estrndup(value, value_size);
		free(value);
	}

	return new;
}

DBA_UPDATE_FUNC(qdbm)
{
	QDBM_DATA;

	if (dpput(dba->dbf, key, keylen, val, vallen, mode == 1 ? DP_DKEEP : DP_DOVER)) {
		return SUCCESS;
	}

	if (dpecode != DP_EKEEP) {
		php_error_docref2(NULL, key, val, E_WARNING, "%s", dperrmsg(dpecode));
	}

	return FAILURE;
}

DBA_EXISTS_FUNC(qdbm)
{
	QDBM_DATA;
	char *value;

	value = dpget(dba->dbf, key, keylen, 0, -1, NULL);
	if (value) {
		free(value);
		return SUCCESS;
	}

	return FAILURE;
}

DBA_DELETE_FUNC(qdbm)
{
	QDBM_DATA;

	return dpout(dba->dbf, key, keylen) ? SUCCESS : FAILURE;
}

DBA_FIRSTKEY_FUNC(qdbm)
{
	QDBM_DATA;
	int value_size;
	char *value, *new = NULL;

	dpiterinit(dba->dbf);

	value = dpiternext(dba->dbf, &value_size);
	if (value) {
		if (newlen) *newlen = value_size;
		new = estrndup(value, value_size);
		free(value);
	}

	return new;
}

DBA_NEXTKEY_FUNC(qdbm)
{
	QDBM_DATA;
	int value_size;
	char *value, *new = NULL;

	value = dpiternext(dba->dbf, &value_size);
	if (value) {
		if (newlen) *newlen = value_size;
		new = estrndup(value, value_size);
		free(value);
	}

	return new;
}

DBA_OPTIMIZE_FUNC(qdbm)
{
	QDBM_DATA;

	dpoptimize(dba->dbf, 0);
	return SUCCESS;
}

DBA_SYNC_FUNC(qdbm)
{
	QDBM_DATA;

	dpsync(dba->dbf);
	return SUCCESS;
}

DBA_INFO_FUNC(qdbm)
{
	return estrdup(dpversion);
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
