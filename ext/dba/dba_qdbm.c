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
  | Author: Marcin Gibula <mg@iceni.pl>                                  |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef DBA_QDBM
#include "php_qdbm.h"

#ifdef QDBM_INCLUDE_FILE
#include QDBM_INCLUDE_FILE
#endif

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
	dba_qdbm_data *dba = info->dbf;

	dpclose(dba->dbf);
	pefree(dba, info->flags & DBA_PERSISTENT);
}

DBA_FETCH_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;
	char *value;
	int value_size;
	zend_string *fetched_val = NULL;

	value = dpget(dba->dbf, ZSTR_VAL(key), ZSTR_LEN(key), 0, -1, &value_size);
	if (value) {
		fetched_val = zend_string_init(value, value_size, /* persistent */ false);
		free(value);
	}

	return fetched_val;
}

DBA_UPDATE_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;

	if (dpput(dba->dbf, ZSTR_VAL(key), ZSTR_LEN(key), ZSTR_VAL(val), ZSTR_LEN(val), mode == 1 ? DP_DKEEP : DP_DOVER)) {
		return SUCCESS;
	}

	if (dpecode != DP_EKEEP) {
		php_error_docref(NULL, E_WARNING, "%s", dperrmsg(dpecode));
	}

	return FAILURE;
}

DBA_EXISTS_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;
	char *value;

	value = dpget(dba->dbf, ZSTR_VAL(key), ZSTR_LEN(key), 0, -1, NULL);
	if (value) {
		free(value);
		return SUCCESS;
	}

	return FAILURE;
}

DBA_DELETE_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;

	return dpout(dba->dbf, ZSTR_VAL(key), ZSTR_LEN(key)) ? SUCCESS : FAILURE;
}

DBA_FIRSTKEY_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;
	int value_size;
	char *value;
	zend_string *key = NULL;

	dpiterinit(dba->dbf);

	value = dpiternext(dba->dbf, &value_size);
	if (value) {
		key = zend_string_init(value, value_size, /* persistent */ false);
		free(value);
	}

	return key;
}

DBA_NEXTKEY_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;
	int value_size;
	char *value;
	zend_string *key = NULL;

	value = dpiternext(dba->dbf, &value_size);
	if (value) {
		key = zend_string_init(value, value_size, /* persistent */ false);
		free(value);
	}

	return key;
}

DBA_OPTIMIZE_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;

	dpoptimize(dba->dbf, 0);
	return SUCCESS;
}

DBA_SYNC_FUNC(qdbm)
{
	dba_qdbm_data *dba = info->dbf;

	dpsync(dba->dbf);
	return SUCCESS;
}

DBA_INFO_FUNC(qdbm)
{
	return estrdup(dpversion);
}

#endif
