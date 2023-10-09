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
   | Author:  Michael Maclean <mgdm@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef DBA_TCADB
#include "php_tcadb.h"

#ifdef TCADB_INCLUDE_FILE
#include TCADB_INCLUDE_FILE
#endif

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
				spprintf(&path_string, 0, "%s#mode=r", ZSTR_VAL(info->path));
				break;
			case DBA_WRITER:
				spprintf(&path_string, 0, "%s#mode=w", ZSTR_VAL(info->path));
				break;
			case DBA_CREAT:
				spprintf(&path_string, 0, "%s#mode=wc", ZSTR_VAL(info->path));
				break;
			case DBA_TRUNC:
				spprintf(&path_string, 0, "%s#mode=wct", ZSTR_VAL(info->path));
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
	dba_tcadb_data *dba = info->dbf;

	tcadbclose(dba->tcadb);
	tcadbdel(dba->tcadb);
	pefree(dba, info->flags & DBA_PERSISTENT);
}

DBA_FETCH_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;
	char *value;
	int value_size;
	zend_string *fetched_val = NULL;

	value = tcadbget(dba->tcadb, ZSTR_VAL(key), ZSTR_LEN(key), &value_size);
	if (value) {
		fetched_val = zend_string_init(value, value_size, /* persistent */ false);
		tcfree(value);
	}

	return fetched_val;
}

DBA_UPDATE_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;
	int result;

	if (mode == 1) {
		/* Insert */
		if (tcadbvsiz(dba->tcadb, ZSTR_VAL(key), ZSTR_LEN(key)) > -1) {
			return FAILURE;
		}
	}

	result = tcadbput(dba->tcadb, ZSTR_VAL(key), ZSTR_LEN(key), ZSTR_VAL(val), ZSTR_LEN(val));

	if (result) {
		return SUCCESS;
	}

	php_error_docref(NULL, E_WARNING, "Error updating data");
	return FAILURE;
}

DBA_EXISTS_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;
	char *value;
	int value_len;

	value = tcadbget(dba->tcadb, ZSTR_VAL(key), ZSTR_LEN(key), &value_len);
	if (value) {
		tcfree(value);
		return SUCCESS;
	}

	return FAILURE;
}

DBA_DELETE_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;

	return tcadbout(dba->tcadb, ZSTR_VAL(key), ZSTR_LEN(key)) ? SUCCESS : FAILURE;
}

DBA_FIRSTKEY_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;
	int value_size;
	char *value;
	zend_string *key = NULL;

	tcadbiterinit(dba->tcadb);

	value = tcadbiternext(dba->tcadb, &value_size);
	if (value) {
		key = zend_string_init(value, value_size, /* persistent */ false);
		tcfree(value);
	}

	return key;
}

DBA_NEXTKEY_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;
	int value_size;
	char *value;
	zend_string *key = NULL;

	value = tcadbiternext(dba->tcadb, &value_size);
	if (value) {
		key = zend_string_init(value, value_size, /* persistent */ false);
		tcfree(value);
	}

	return key;
}

DBA_OPTIMIZE_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;

#if _TC_LIBVER >= 811
	return tcadboptimize(dba->tcadb, NULL) ? SUCCESS : FAILURE;
#else
	return FAILURE;
#endif
}

DBA_SYNC_FUNC(tcadb)
{
	dba_tcadb_data *dba = info->dbf;

	return tcadbsync(dba->tcadb) ? SUCCESS : FAILURE;
}

DBA_INFO_FUNC(tcadb)
{
	return estrdup(tcversion);
}

#endif
