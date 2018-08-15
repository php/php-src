/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if DBA_INIFILE
#include "php_inifile.h"

#include "libinifile/inifile.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define INIFILE_DATA \
	inifile *dba = info->dbf

#define INIFILE_GKEY \
	key_type ini_key; \
	if (!key) { \
		php_error_docref(NULL, E_WARNING, "No key specified"); \
		return 0; \
	} \
	ini_key = inifile_key_split((char*)key) /* keylen not needed here */

#define INIFILE_DONE \
	inifile_key_free(&ini_key)

DBA_OPEN_FUNC(inifile)
{
	info->dbf = inifile_alloc(info->fp, info->mode == DBA_READER, info->flags&DBA_PERSISTENT);

	return info->dbf ? SUCCESS : FAILURE;
}

DBA_CLOSE_FUNC(inifile)
{
	INIFILE_DATA;

	inifile_free(dba, info->flags&DBA_PERSISTENT);
}

DBA_FETCH_FUNC(inifile)
{
	val_type ini_val;

	INIFILE_DATA;
	INIFILE_GKEY;

	ini_val = inifile_fetch(dba, &ini_key, skip);
	*newlen = ini_val.value ? strlen(ini_val.value) : 0;
	INIFILE_DONE;
	return ini_val.value;
}

DBA_UPDATE_FUNC(inifile)
{
	val_type ini_val;
	int res;

	INIFILE_DATA;
	INIFILE_GKEY;

	ini_val.value = val;

	if (mode == 1) {
		res = inifile_append(dba, &ini_key, &ini_val);
	} else {
		res = inifile_replace(dba, &ini_key, &ini_val);
	}
	INIFILE_DONE;
	switch(res) {
	case -1:
		php_error_docref1(NULL, key, E_WARNING, "Operation not possible");
		return FAILURE;
	default:
	case 0:
		return SUCCESS;
	case 1:
		return FAILURE;
	}
}

DBA_EXISTS_FUNC(inifile)
{
	val_type ini_val;

	INIFILE_DATA;
	INIFILE_GKEY;

	ini_val = inifile_fetch(dba, &ini_key, 0);
	INIFILE_DONE;
	if (ini_val.value) {
		inifile_val_free(&ini_val);
		return SUCCESS;
	}
	return FAILURE;
}

DBA_DELETE_FUNC(inifile)
{
	int res;
	zend_bool found = 0;

	INIFILE_DATA;
	INIFILE_GKEY;

	res =  inifile_delete_ex(dba, &ini_key, &found);

	INIFILE_DONE;
	return (res == -1 || !found ? FAILURE : SUCCESS);
}

DBA_FIRSTKEY_FUNC(inifile)
{
	INIFILE_DATA;

	if (inifile_firstkey(dba)) {
		char *result = inifile_key_string(&dba->curr.key);
		*newlen = strlen(result);
		return result;
	} else {
		return NULL;
	}
}

DBA_NEXTKEY_FUNC(inifile)
{
	INIFILE_DATA;

	if (!dba->curr.key.group && !dba->curr.key.name) {
		return NULL;
	}

	if (inifile_nextkey(dba)) {
		char *result = inifile_key_string(&dba->curr.key);
		*newlen = strlen(result);
		return result;
	} else {
		return NULL;
	}
}

DBA_OPTIMIZE_FUNC(inifile)
{
	/* dummy */
	return SUCCESS;
}

DBA_SYNC_FUNC(inifile)
{
	/* dummy */
	return SUCCESS;
}

DBA_INFO_FUNC(inifile)
{
	return estrdup(inifile_version());
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
